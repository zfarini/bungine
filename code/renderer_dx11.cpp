Shader load_shaderString filename, ShaderType type, const char *main = "")
{
    Shader result = {};

    ID3DBlob *shader_blob = 0;

    ID3DBlob *error_blob;

    const char *version = 0;

    if (type == SHADER_TYPE_VERTEX)
        version = "vs_5_0";
    else if (type == SHADER_TYPE_FRAGMENT)
        version = "ps_5_0";
    else
        assert(0);

    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS|D3DCOMPILE_DEBUG;

	if (FAILED(D3DCompileFromFile(filename, 0,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, main, version,
			flags, 0, &shader_blob, &error_blob))) {
		printf("DX11: failed to compiled shader %.*s: %s\n", str_format(filename), (char *)error_blob->GetBufferPointer());
        assert(0);
    }

    result.handle = (void *)shader_blob;
    return result;
}

ShaderProgram create_shader_programShader vs, Shader ps)
{
    ShaderProgram result = {};

    result.ps = ps;
    result.fs = fs;
    return result;
}

void set_primitive_type(int type)
{
	g_rc.render_pass->primitive_type = type;

    int topology;
	if (type == PRIMITIVE_TRIANGLES)
		topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	else if (type == PRIMITIVE_LINES)
		topology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	else
		assert(0);

	g_rc.context->IASetPrimitiveTopology(topology);
}

RenderPass create_render_pass(ShaderProgram program,
	VertexInputElement *input_elements, int input_element_count)
{
	RenderPass result = {};

	assert(input_element_count < ARRAY_SIZE(result.input_elements));
	result.program = program;
	result.input_element_count = input_element_count;
	for (int i = 0; i < input_element_count; i++)
		result.input_elements[i] = input_elements[i];

	set_primitive_type(result, PRIMITIVE_TRIANGLES);

	// {
    //     D3D11_RASTERIZER_DESC desc = {};
    //     desc.FillMode = D3D11_FILL_SOLID;
    //     desc.CullMode = D3D11_CULL_BACK;
	// 	// NOTE: this seems to be for enabling depth clipping for the range specified in viewport
	// 	desc.DepthClipEnable = TRUE;
        
    //     rc.device->CreateRasterizerState(&desc, &rp.rasterizer_state);
    // }
	D3D11_INPUT_ELEMENT_DESC input_layout_desc[ARRAY_SIZE(result.input_elements)] = {};
	for (int i = 0; i < input_element_count; i++) {
		input_layout_desc[i].SemanticName = input_elements[i].name;
		input_layout_desc[i].Format = ;
		input_layout_desc[i].AlignedByteOffset = input_elements[i].offset;
		input_layout_desc[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	}
	
	g_rc.device->CreateInputLayout(input_layout_desc, input_layout_count,
			((ID3DBlob *)program.vs.handle)->GetBufferPointer(),
			((ID3DBlob *)program.vs.handle)->GetBufferSize(),
			&result.input_layout);

	return result;
}