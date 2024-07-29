#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")
#pragma comment (lib, "dxgi")
#pragma comment (lib, "dxguid")

#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d11.h>
#include <dxgi1_3.h>
#include <d3dcompiler.h>
#include <dxgidebug.h>
#undef min
#undef max
#undef near
#undef swap

Texture create_texture(String name, void *data, int width, int height, bool srgb = true,
		bool mipmapping = true)
{
	Texture result = {};

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width              = width;
	desc.Height             = height;
	desc.MipLevels          = mipmapping ? 0 : 1;
	desc.ArraySize          = 1;
	desc.Format				= DXGI_FORMAT_R8G8B8A8_UNORM;
	if (srgb)
		desc.Format         = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
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

	g_rc->device->CreateTexture2D(&desc, 0, &texture);
	g_rc->context->UpdateSubresource(texture, 0, 0, s_data.pSysMem, s_data.SysMemPitch, 0);

	ID3D11ShaderResourceView *srv;

	D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
	//srv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv_desc.Texture2D.MipLevels = mipmapping ? (UINT)-1 : 1;

	g_rc->device->CreateShaderResourceView(texture, &srv_desc, &srv);
	if (mipmapping)
		g_rc->context->GenerateMips(srv);

	texture->Release();

	result.srv = srv;
	result.name = name;
	result.valid = true;
	return result;
}

void bind_texture(int index, Texture &texture)
{
	g_rc->context->PSSetShaderResources(index, 1, &texture.srv);
}

Shader load_shader(String filename, ShaderType type, const char *main = "")
{
	Shader result = {};

	result.type = type;

	ID3DBlob *blob = 0;

	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;

#ifdef RENDERER_DEBUG
	flags |= D3DCOMPILE_DEBUG;
#endif

	wchar_t wfilename[4096];  
	MultiByteToWideChar(CP_ACP, 0, filename.data, (int)filename.count + 1, wfilename, sizeof(wfilename));

	const char *version = 0;

	if (type == SHADER_TYPE_VERTEX)
		version = "vs_5_0";
	else if (type == SHADER_TYPE_FRAGMENT)
		version = "ps_5_0";
	else
		assert(0);

	ID3DBlob *error_blob;
	if (FAILED(D3DCompileFromFile(wfilename, 0,
					D3D_COMPILE_STANDARD_FILE_INCLUDE, main, version,
					flags, 0, &blob, &error_blob))) {
		OutputDebugStringA((char *)error_blob->GetBufferPointer());
		assert(0);
	}
	if (type == SHADER_TYPE_VERTEX)
		g_rc->device->CreateVertexShader(blob->GetBufferPointer(), 
				blob->GetBufferSize(), 0, &result.vs);
	else if (type == SHADER_TYPE_FRAGMENT)
		g_rc->device->CreatePixelShader(blob->GetBufferPointer(), 
				blob->GetBufferSize(), 0, &result.ps);
	else
		assert(0);

	result.blob = blob;

	return result;
}

RasterizerState create_rasterizer_state(RasterizerFillMode fillmode, RasterizerCullMode cullmode)
{	
	RasterizerState result = {};

	D3D11_RASTERIZER_DESC desc = {};
	if (fillmode == RASTERIZER_FILL_SOLID)
		desc.FillMode = D3D11_FILL_SOLID;
	else if (fillmode == RASTERIZER_FILL_WIREFRAME)
		desc.FillMode = D3D11_FILL_WIREFRAME;
	else
		assert(0);

	if (cullmode == RASTERIZER_CULL_NONE)
		desc.CullMode = D3D11_CULL_NONE;
	else if (cullmode == RASTERIZER_CULL_FRONT)
		desc.CullMode = D3D11_CULL_FRONT;
	else if (cullmode == RASTERIZER_CULL_BACK)
		desc.CullMode = D3D11_CULL_BACK;
	else
		assert(0);

	desc.FrontCounterClockwise = TRUE;
	// NOTE: this seems to be for enabling depth clipping for the range specified in viewport
	desc.DepthClipEnable = TRUE;
	g_rc->device->CreateRasterizerState(&desc, &result.state);
	return result;
}

RenderPass create_render_pass(Shader vs, Shader fs, Array<VertexInputElement> input_elements,
		PrimitiveType primitive_type, DepthStencilState &depth_stencil_state,
		RasterizerState &rasterizer_state)
{
	RenderPass rp = {};

	rp.vs = vs;
	rp.fs = fs;
	rp.depth_stencil_state = depth_stencil_state;
	rp.rasterizer_state = rasterizer_state;
	rp.primitive_type = primitive_type;

	D3D11_INPUT_ELEMENT_DESC input_layout_desc[32];
	assert(input_elements.count < ARRAY_SIZE(input_layout_desc));
	for (usize i = 0; i < input_elements.count; i++) {
		input_layout_desc[i].SemanticName = input_elements[i].name;	
		input_layout_desc[i].SemanticIndex = 0;

		int count = input_elements[i].count;
		DXGI_FORMAT format = DXGI_FORMAT_R32G32B32_FLOAT;
		if (input_elements[i].type == INPUT_ELEMENT_FLOAT) {
			if (count == 1) format = DXGI_FORMAT_R32_FLOAT;
			else if (count == 2) format = DXGI_FORMAT_R32G32_FLOAT;
			else if (count == 3) format = DXGI_FORMAT_R32G32B32_FLOAT;
			else if (count == 4) format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			else
				assert(0);
		}
		else if (input_elements[i].type == INPUT_ELEMENT_SIGNED_INT) {
			if (count == 1) format = DXGI_FORMAT_R32_SINT;
			else if (count == 2) format = DXGI_FORMAT_R32G32_SINT;
			else if (count == 3) format = DXGI_FORMAT_R32G32B32_SINT;
			else if (count == 4) format = DXGI_FORMAT_R32G32B32A32_SINT;
			else
				assert(0);	
		}
		else
			assert(0);
		input_layout_desc[i].Format = format;
		input_layout_desc[i].InputSlot = 0;
		input_layout_desc[i].AlignedByteOffset = input_elements[i].offset;
		input_layout_desc[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		input_layout_desc[i].InstanceDataStepRate = 0;
	}

	g_rc->device->CreateInputLayout(input_layout_desc, (UINT)input_elements.count,
			vs.blob->GetBufferPointer(), 
			vs.blob->GetBufferSize(), &rp.input_layout);

	return rp;
}

DepthStencilState create_depth_stencil_state(bool enable_depth)
{
	DepthStencilState result = {};

	result.enable_depth = enable_depth;

	D3D11_DEPTH_STENCIL_DESC desc = {};
	desc.DepthEnable    = enable_depth;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	desc.DepthFunc      = D3D11_COMPARISON_LESS;
	g_rc->device->CreateDepthStencilState(&desc, &result.state);
	return result;
}

void begin_render_pass(RenderPass &rp)
{
	g_rc->render_pass = &rp;

	if (rp.primitive_type == PRIMITIVE_TRIANGLES)
		g_rc->context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	else if (rp.primitive_type == PRIMITIVE_LINES)
		g_rc->context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	else
		assert(0);
	g_rc->context->IASetInputLayout(rp.input_layout);
	g_rc->context->VSSetShader(rp.vs.vs, 0, 0);
	g_rc->context->PSSetShader(rp.fs.ps, 0, 0);

	g_rc->context->OMSetDepthStencilState(rp.depth_stencil_state.state, 0);
	g_rc->context->RSSetState(rp.rasterizer_state.state);
}

void end_render_pass()
{
	g_rc->render_pass = 0;
}

void set_viewport(float top_left_x, float top_left_y, float width, float height)
{
	D3D11_VIEWPORT viewport;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	viewport.Width = width;
	viewport.Height = height;
	viewport.TopLeftX = top_left_x;
	viewport.TopLeftY = top_left_y;

	g_rc->context->RSSetViewports(1, &viewport);
}

void bind_framebuffer(FrameBuffer &framebuffer)
{
	g_rc->context->OMSetRenderTargets(1, &framebuffer.rtv, framebuffer.dsv);
}

void clear_framebuffer_color(FrameBuffer &framebuffer, v4 color)
{
	g_rc->context->ClearRenderTargetView(framebuffer.rtv, (FLOAT *)color.e);
}

void clear_framebuffer_depth(FrameBuffer &framebuffer, float depth)
{
	g_rc->context->ClearDepthStencilView(framebuffer.dsv, D3D11_CLEAR_DEPTH, depth, 0);
}

VertexBuffer create_vertex_buffer(VertexBufferUsage usage, usize vertex_size, usize size, void *data = 0)
{
	VertexBuffer result = {};

	result.usage = usage;
	result.size = size;
	result.vertex_size = vertex_size;

	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = (UINT)size;
	if (usage == VERTEX_BUFFER_IMMUTABLE)
		desc.Usage = D3D11_USAGE_IMMUTABLE;
	else if (usage == VERTEX_BUFFER_DYNAMIC) {
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}

	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	if (!data) {
		assert(usage == VERTEX_BUFFER_DYNAMIC);
		g_rc->device->CreateBuffer(&desc, 0, &result.buffer);
	}
	else {
		D3D11_SUBRESOURCE_DATA init_data = {};
		init_data.pSysMem = data;
		g_rc->device->CreateBuffer(&desc, &init_data, &result.buffer);
	}
	return result;
}

void update_vertex_buffer(VertexBuffer &vb, int size, void *data)
{
	assert(vb.usage == VERTEX_BUFFER_DYNAMIC);

	D3D11_MAPPED_SUBRESOURCE buffer_mapped;
	g_rc->context->Map(vb.buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &buffer_mapped);
	memcpy(buffer_mapped.pData, data, size);
	g_rc->context->Unmap(vb.buffer, 0);
}

void bind_vertex_buffer(VertexBuffer &vb)
{
	UINT stride = (UINT)vb.vertex_size;
	UINT offset = 0;
	g_rc->context->IASetVertexBuffers(0, 1, &vb.buffer, &stride, &offset);
}

void draw(int offset, int vertices_count)
{
	g_rc->context->Draw((UINT)vertices_count, (UINT)offset);
}


int get_type_alignement(ConstantBufferElementType type)
{
	return 4;
}

int get_type_size(ConstantBufferElementType type)
{
	switch (type) {
		case CONSTANT_BUFFER_ELEMENT_MAT4: return sizeof(mat4);
		case CONSTANT_BUFFER_ELEMENT_VEC4: return sizeof(v4);
		case CONSTANT_BUFFER_ELEMENT_VEC3: return sizeof(v3);
		case CONSTANT_BUFFER_ELEMENT_VEC2: return sizeof(v2);
		case CONSTANT_BUFFER_ELEMENT_FLOAT: return sizeof(float);
		case CONSTANT_BUFFER_ELEMENT_INT: return sizeof(int);
		default: assert(0);
	}
	return 0;
}

//https://maraneshi.github.io/HLSL-ConstantBufferLayoutVisualizer/
ConstantBuffer create_constant_buffer(Array<ConstantBufferElement> elements)
{
	ConstantBuffer result = {};

	assert(elements.count < ARRAY_SIZE(result.elements));
	int offset = 0;
	// TODO: actually test this
	for (usize i = 0; i < elements.count; i++) {
		int type_align = get_type_alignement(elements[i].type);
		int type_size  = get_type_size(elements[i].type);

		if (elements[i].array_size) {
			offset = align_to(offset, 16);
			offset += (elements[i].array_size - 1) * align_to(type_size, 16);
			offset += type_size;
		}
		else {
			offset = align_to(offset, type_align);
			if (offset + type_size > align_to(offset, 16))
				offset = align_to(offset, 16);
			offset += type_size;
		}

		result.elements[i] = elements[i];
	}
	result.element_count = (int)elements.count;
	result.size = align_to(offset, 16);

	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = (UINT)result.size;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	g_rc->device->CreateBuffer(&desc, 0, &result.buffer);

	return result;
}

void update_constant_buffer(ConstantBuffer &buffer, void *data)
{
	Arena *temp = begin_temp_memory();

	void *cdata = arena_alloc(temp, buffer.size);
	memset(cdata, 0, buffer.size);

	int offset = 0;
	int data_offset = 0;

	for (int i = 0; i < buffer.element_count; i++) {
		int type_align = get_type_alignement(buffer.elements[i].type);
		int type_size  = get_type_size(buffer.elements[i].type);

		if (buffer.elements[i].array_size) {
			data_offset = align_to(data_offset, get_c_type_alignement(buffer.elements[i].type));
			for (int j = 0; j < buffer.elements[i].array_size; j++) {
				offset = align_to(offset, 16);
				memcpy((char *)cdata + offset, (char *)data + data_offset, type_size);
				data_offset += type_size;
				offset += type_size;
			}
		}
		else {
			data_offset = align_to(data_offset, get_c_type_alignement(buffer.elements[i].type));

			offset = align_to(offset, type_align);
			if (offset + type_size > align_to(offset, 16))
				offset = align_to(offset, 16);
			memcpy((char *)cdata + offset, (char *)data + data_offset, type_size);
			offset += type_size;
			data_offset += type_size;
		}

	}

	D3D11_MAPPED_SUBRESOURCE buffer_mapped;
	g_rc->context->Map(buffer.buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &buffer_mapped);
	memcpy(buffer_mapped.pData, cdata, buffer.size);
	g_rc->context->Unmap(buffer.buffer, 0);

	end_temp_memory();
}

void bind_constant_buffer(ConstantBuffer &cbuffer)
{
	g_rc->context->VSSetConstantBuffers(0, 1, &cbuffer.buffer);
	g_rc->context->PSSetConstantBuffers(0, 1, &cbuffer.buffer);
}

void begin_render_frame()
{
	RECT rect;
	GetClientRect(g_rc->window, &rect);
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

	if (width != g_rc->window_width || height != g_rc->window_height) {
		g_rc->window_width = width;
		g_rc->window_height = height;

		g_rc->context->ClearState();
		g_rc->window_framebuffer.rtv->Release();
		g_rc->window_framebuffer.dsv->Release();

		g_rc->window_framebuffer.rtv = 0;
		g_rc->window_framebuffer.dsv = 0;

		if (width != 0 && height != 0) {
			if (FAILED(g_rc->swap_chain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0)))
				assert(0);


			// create RenderTarget view for new backbuffer texture
			ID3D11Texture2D* backbuffer;
			g_rc->swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backbuffer);
			D3D11_RENDER_TARGET_VIEW_DESC backbuffer_desc = {};
			backbuffer_desc.Format        = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
			backbuffer_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			g_rc->device->CreateRenderTargetView((ID3D11Resource*)backbuffer, &backbuffer_desc, &g_rc->window_framebuffer.rtv);
			backbuffer->Release();

			D3D11_TEXTURE2D_DESC depth_buffer_desc;
			backbuffer->GetDesc(&depth_buffer_desc);
			depth_buffer_desc.Format = DXGI_FORMAT_D32_FLOAT;
			depth_buffer_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

			// create new depth stencil texture & DepthStencil view
			ID3D11Texture2D* depth_buffer;
			g_rc->device->CreateTexture2D(&depth_buffer_desc, NULL, &depth_buffer);
			g_rc->device->CreateDepthStencilView((ID3D11Resource*)depth_buffer, NULL, &g_rc->window_framebuffer.dsv);
			depth_buffer->Release();
		}
	}
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	g_rc->debug_lines.count = 0;
}

void end_render_frame()
{
	ImGui::Render();
	g_rc->context->OMSetRenderTargets(1, &g_rc->window_framebuffer.rtv, 0);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	g_rc->swap_chain->Present(1, 0);
}

void init_render_context_dx11(RenderContext &rc, HWND window)
{
	rc.window = window;

	{
		u32 flags = D3D11_CREATE_DEVICE_SINGLETHREADED;

#ifdef RENDERER_DEBUG
		flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		DXGI_SWAP_CHAIN_DESC desc = {};
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		// TODO: running on vsync with 2 buffers lags a lot
		// I don't know why yet
		desc.BufferCount = 3;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.OutputWindow = window;
		desc.SampleDesc.Count = 1;
		desc.Windowed = 1;
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;


		assert(SUCCEEDED(D3D11CreateDeviceAndSwapChain(0, D3D_DRIVER_TYPE_HARDWARE, 0, flags, 0, 0, D3D11_SDK_VERSION,
						&desc, &rc.swap_chain, &rc.device, 0, &rc.context)));
	}
#ifdef RENDERER_DEBUG
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
		// TODO: what happens when we have mutiple backbuffers?
		ID3D11Texture2D* backbuffer;
		rc.swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backbuffer);
		D3D11_RENDER_TARGET_VIEW_DESC backbuffer_desc = {};
		backbuffer_desc.Format        = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		backbuffer_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rc.device->CreateRenderTargetView((ID3D11Resource*)backbuffer, &backbuffer_desc, 
				&rc.window_framebuffer.rtv);

		{
			D3D11_TEXTURE2D_DESC desc;
			backbuffer->GetDesc(&desc);
			rc.window_width = desc.Width;
			rc.window_height = desc.Height;
		}

		ID3D11Texture2D *depth_buffer;

		D3D11_TEXTURE2D_DESC depth_buffer_desc;
		backbuffer->GetDesc(&depth_buffer_desc);
		depth_buffer_desc.Format = DXGI_FORMAT_D32_FLOAT;
		depth_buffer_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		rc.device->CreateTexture2D(&depth_buffer_desc, 0, &depth_buffer);
		rc.device->CreateDepthStencilView(depth_buffer, 0,  &rc.window_framebuffer.dsv);

		backbuffer->Release();
		depth_buffer->Release();
	}
}
