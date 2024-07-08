
#include "renderer.h"

//#define ENABLE_SRGB 

ID3D11ShaderResourceView *create_texture(RenderContext &rc, void *data, int width, int height, bool srgb = true)
{
	bool mipmapping = true;

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width              = width;
	desc.Height             = height;
	desc.MipLevels          = mipmapping ? 0 : 1;
	desc.ArraySize          = 1;
	desc.Format				= DXGI_FORMAT_R8G8B8A8_UNORM;
	#ifdef ENABLE_SRGB
	if (srgb)
		desc.Format         = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	#endif
	desc.SampleDesc.Count   = 1;
	desc.Usage              = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags          = D3D11_BIND_SHADER_RESOURCE;

	if (mipmapping) {
		desc.Usage = D3D11_USAGE_DEFAULT;
    	desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
    	desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
	}

	D3D11_SUBRESOURCE_DATA s_data = {};
	s_data.pSysMem     = data;
	s_data.SysMemPitch = width * 4;

	ID3D11Texture2D* texture;

	rc.device->CreateTexture2D(&desc, 0, &texture);
    rc.context->UpdateSubresource(texture, 0, 0, s_data.pSysMem, s_data.SysMemPitch, 0);

	ID3D11ShaderResourceView *srv;
	
	D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
	srv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	#ifdef ENABLE_SRGB
	if (srgb)
		srv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	#endif
	srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srv_desc.Texture2D.MipLevels = mipmapping ? (UINT)-1 : 1;

	rc.device->CreateShaderResourceView(texture, &srv_desc, &srv);
	if (mipmapping)
        rc.context->GenerateMips(srv);

	texture->Release();
	return srv;
}

void init_render_context(RenderContext &rc, HWND window)
{
	{
		u32 flags = D3D11_CREATE_DEVICE_SINGLETHREADED;
		
		#ifdef DIRECT3D_DEBUG
			flags |= D3D11_CREATE_DEVICE_DEBUG;
		#endif

		DXGI_SWAP_CHAIN_DESC desc = {};
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.BufferCount = 2;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.OutputWindow = window;
		desc.SampleDesc.Count = 1;
		desc.Windowed = 1;
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;


		assert(SUCCEEDED(D3D11CreateDeviceAndSwapChain(0, D3D_DRIVER_TYPE_HARDWARE, 0, flags, 0, 0, D3D11_SDK_VERSION,
			&desc, &rc.swap_chain, &rc.device, 0, &rc.context)));
	}
	#ifdef DIRECT3D_DEBUG
    {
        ID3D11InfoQueue* info;
        rc.device->QueryInterface(__uuidof(ID3D11InfoQueue), (void**)&info);
        info->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, TRUE);
        info->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, TRUE);
        info->Release();
    }

    // enable debug break for DXGI too
    {
        IDXGIInfoQueue* dxgiInfo;
        DXGIGetDebugInterface1(0, __uuidof(IDXGIInfoQueue), (void**)&dxgiInfo);
        dxgiInfo->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, TRUE);
        dxgiInfo->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, TRUE);
        dxgiInfo->Release();
    }
	#endif

	{
		uint32_t data = 0xffffffff;
		rc.white_texture = create_texture(rc, &data, 1, 1, false);
	}

		{
		ID3D11Texture2D* backbuffer;
        rc.swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backbuffer);
		D3D11_RENDER_TARGET_VIEW_DESC backbuffer_desc = {};
		#ifdef ENABLE_SRGB
    	backbuffer_desc.Format        = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    	#else
		backbuffer_desc.Format        = DXGI_FORMAT_R8G8B8A8_UNORM;
		#endif
		backbuffer_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        rc.device->CreateRenderTargetView((ID3D11Resource*)backbuffer, &backbuffer_desc, &rc.backbuffer_rtv);

		ID3D11Texture2D *depth_buffer;
		
		D3D11_TEXTURE2D_DESC depth_buffer_desc;
		backbuffer->GetDesc(&depth_buffer_desc);
		depth_buffer_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depth_buffer_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		rc.device->CreateTexture2D(&depth_buffer_desc, 0, &depth_buffer);
		rc.device->CreateDepthStencilView(depth_buffer, 0, &rc.depth_stencil_view);

        backbuffer->Release();
		depth_buffer->Release();
	}
	rc.loaded_textures = make_array_max<Texture>(512);
}

RenderPass create_render_pass(RenderContext &rc, LPCWSTR shader_filename,
	const char *vs_main, const char *ps_main,
	D3D11_INPUT_ELEMENT_DESC *input_layout_desc, int input_layout_count,
	usize constant_buffer_size)
{
	RenderPass rp = {};

	ID3DBlob *vertex_shader_blob = 0;
	ID3DBlob *pixel_shader_blob = 0;
	
	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
	
	#ifdef DIRECT3D_DEBUG
	flags |= D3DCOMPILE_DEBUG;
	#endif

	ID3DBlob *error_blob;
	if (FAILED(D3DCompileFromFile(shader_filename, 0,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, vs_main, "vs_5_0",
			flags, 0, &vertex_shader_blob, &error_blob))) {
		OutputDebugStringA((char *)error_blob->GetBufferPointer());
		assert(0);
	}
	if (ps_main) {
		if (FAILED(D3DCompileFromFile(shader_filename, 0,
				D3D_COMPILE_STANDARD_FILE_INCLUDE, ps_main, "ps_5_0",
				flags, 0, &pixel_shader_blob, &error_blob))) {
			OutputDebugStringA((char *)error_blob->GetBufferPointer());
			assert(0);
		}
	}

	rc.device->CreateVertexShader(vertex_shader_blob->GetBufferPointer(), 
		vertex_shader_blob->GetBufferSize(), 0, &rp.vertex_shader);
	if (ps_main)
		rc.device->CreatePixelShader(pixel_shader_blob->GetBufferPointer(), 
			pixel_shader_blob->GetBufferSize(), 0, &rp.pixel_shader);

	rc.device->CreateInputLayout(input_layout_desc, input_layout_count,
			vertex_shader_blob->GetBufferPointer(), 
			vertex_shader_blob->GetBufferSize(), &rp.input_layout);
	
	 {
        D3D11_BUFFER_DESC desc = {};
		desc.ByteWidth = (UINT)constant_buffer_size;
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        rc.device->CreateBuffer(&desc, 0, &rp.cbuffer);
    }
	{
        D3D11_RASTERIZER_DESC desc = {};
        desc.FillMode = D3D11_FILL_SOLID;
        desc.CullMode = D3D11_CULL_BACK;
		// NOTE: this seems to be for enabling depth clipping for the range specified in viewport
		desc.DepthClipEnable = TRUE;
        
        rc.device->CreateRasterizerState(&desc, &rp.rasterizer_state);
    }
	{
		D3D11_SAMPLER_DESC desc = {};
    	desc.Filter         = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    	desc.AddressU       = D3D11_TEXTURE_ADDRESS_WRAP;
    	desc.AddressV       = D3D11_TEXTURE_ADDRESS_WRAP;
    	desc.AddressW       = D3D11_TEXTURE_ADDRESS_WRAP;
    	desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.MipLODBias = 0.0f;
        desc.MaxAnisotropy = 1;
        desc.MinLOD = 0;
        desc.MaxLOD = D3D11_FLOAT32_MAX;
    	rc.device->CreateSamplerState(&desc, &rp.sampler_state);
	}
	{
		D3D11_DEPTH_STENCIL_DESC desc = {};
    	desc.DepthEnable    = TRUE;
    	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    	desc.DepthFunc      = D3D11_COMPARISON_LESS;

    	rc.device->CreateDepthStencilState(&desc, &rp.depth_stencil_state);
	}

	vertex_shader_blob->Release();
	if (ps_main)
		pixel_shader_blob->Release();

	return rp;
}

void begin_render_pass(RenderContext &rc, RenderPass &rp, D3D11_VIEWPORT viewport,
	ID3D11RenderTargetView *rtv, ID3D11DepthStencilView *dsv,
	mat4 view_mat, mat4 projection_mat)
{
	rc.context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	rc.context->IASetInputLayout(rp.input_layout);

	rc.context->RSSetState(rp.rasterizer_state);
	rc.context->RSSetViewports(1, &viewport);

	rc.context->VSSetShader(rp.vertex_shader, 0, 0);
	rc.context->VSSetConstantBuffers(0, 1, &rp.cbuffer);

	rc.context->PSSetShader(rp.pixel_shader, 0, 0);
	rc.context->PSSetSamplers(0, 1, &rp.sampler_state);
	rc.context->PSSetConstantBuffers(0, 1, &rp.cbuffer);

	if (rtv)
		rc.context->OMSetRenderTargets(1, &rtv, dsv);
	else
		rc.context->OMSetRenderTargets(0, 0, dsv);

	rc.context->OMSetDepthStencilState(rp.depth_stencil_state, 0);

	rp.view_mat = view_mat;
	rp.projection_mat = projection_mat;

	rc.render_pass = &rp;
}

void end_render_pass(RenderContext &rc, RenderPass &rp)
{
	rc.render_pass = 0;
}

void begin_frame(RenderContext &rc)
{
	
}

void end_frame(RenderContext &rc)
{
	rc.swap_chain->Present(1, 0);
}

void copy_constants_to_gpu(RenderContext &rc, ID3D11Buffer *cbuffer, void *data, usize size)
{
	D3D11_MAPPED_SUBRESOURCE constant_buffer_msr;
	rc.context->Map(cbuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &constant_buffer_msr);
	memcpy(constant_buffer_msr.pData, data, size);
	rc.context->Unmap(cbuffer, 0);
}

ID3D11Buffer *create_vertex_buffer(RenderContext &rc, usize size, void *data = 0)
{
	ID3D11Buffer *buffer;

	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = (UINT)size;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	if (data) {
		D3D11_SUBRESOURCE_DATA initial = {};
		initial.pSysMem = data;
		rc.device->CreateBuffer(&desc, &initial, &buffer);
	}
	else
		rc.device->CreateBuffer(&desc, 0, &buffer);
	return buffer;
}