

LoadedSound load_wav_file(Arena *arena, const char *filename)
{
    ma_decoder_config config = ma_decoder_config_init(ma_format_f32, SOUND_CHANNEL_COUNT, SOUND_SAMPLE_RATE);

    ma_decoder decoder;
    ma_result result = ma_decoder_init_file(filename, &config, &decoder);
    if (result != MA_SUCCESS)
        assert(0);

	// NOTE: just trying to find file size, this shouldn't really happen
	
	LoadedSound sound = {};
    //sound.samples = (float *)calloc(1, 2 * sizeof(float) * 48000 * 60);
    ma_uint64 samplesToRead = 1024;
	// TODO: cleanup !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! alocating 360 seconds for now
	sound.samples = (float *)arena_alloc(arena, 360 * SOUND_SAMPLE_RATE * sizeof(float) * SOUND_CHANNEL_COUNT);

    while (1) {
        ma_uint64 samplesRead = 0;
        result = ma_decoder_read_pcm_frames(&decoder, sound.samples + sound.sample_count*SOUND_CHANNEL_COUNT, 
			samplesToRead, &samplesRead);
        sound.sample_count += samplesRead;
        if (samplesRead < samplesToRead)
            break ;
		// @HACK: kinda, I'm just assuming stuff will be continuous with no random alignement jumps
		//arena_alloc(arena, samplesToRead * sizeof(float) * SOUND_CHANNEL_COUNT);
    }
	LOG_INFO("loaded sound %s, %d samples", filename, sound.sample_count);
    return sound;
}
