#define SOUND_CHANNEL_COUNT 2
#define SOUND_SAMPLE_RATE 48000

LoadedSound load_wav_file(Arena *arena, const char *filename)
{
    ma_decoder_config config = ma_decoder_config_init(ma_format_f32, SOUND_CHANNEL_COUNT, SOUND_SAMPLE_RATE);
	config.encodingFormat = ma_encoding_format_wav;

    ma_decoder decoder;
    ma_result result = ma_decoder_init_file(filename, &config, &decoder);
    if (result != MA_SUCCESS)
        assert(0);

	ma_uint64 samples_to_read = SOUND_SAMPLE_RATE;

	// TOOD: find a better way to find the size
	usize sample_count = 0;
	{
		Arena *temp_arena = begin_temp_memory();
		float *samples = (float *)arena_alloc(temp_arena, samples_to_read * sizeof(float) * SOUND_CHANNEL_COUNT);
		
		 while (1) {
			ma_uint64 samples_read = 0;
			result = ma_decoder_read_pcm_frames(&decoder, samples, samples_to_read, &samples_read);
			sample_count += samples_read;
			if (samples_read < samples_to_read)
				break ;
    	}
		end_temp_memory();
	}
	//ma_decoder_init_file(filename, &config, &decoder);

	LoadedSound sound = {};
	sound.samples = (float *)arena_alloc(arena, sample_count * sizeof(float) * SOUND_CHANNEL_COUNT);
	assert(ma_decoder_seek_to_pcm_frame(&decoder, 0) == MA_SUCCESS);
    while (1) {
        ma_uint64 samples_read = 0;
        result = ma_decoder_read_pcm_frames(&decoder, sound.samples + sound.sample_count*SOUND_CHANNEL_COUNT, 
			samples_to_read, &samples_read);
        sound.sample_count += samples_read;
        if (samples_read < samples_to_read)
            break ;
    }
	ma_decoder_uninit(&decoder);
	LOG_INFO("loaded sound %s, %d samples", filename, sound.sample_count);
    return sound;
}

void audio_write_callback(ma_device* device, void* output, const void* input, ma_uint32 frame_count)
{
	Game &game = *((Game *)device->pUserData);
	if (!game.is_initialized) 
		return ;

	int read_index = game.sound_state.read_index;
	int write_index = game.sound_state.write_index;
	//printf("read: %d %d %d\n", read_index, write_index, game.sound_state.sample_count);

	if (read_index == write_index)
		return ;

	if (read_index < write_index) {
		int samples_to_read = min(write_index - read_index, (int)frame_count);
		memcpy(output, game.sound_state.buffer + read_index * SOUND_CHANNEL_COUNT,
			samples_to_read * SOUND_CHANNEL_COUNT * sizeof(float));
		game.sound_state.read_index = read_index + samples_to_read;
	}
	else {
		int samples_to_copy = min((int)frame_count, game.sound_state.sample_count - read_index);
		memcpy(output, game.sound_state.buffer + read_index * SOUND_CHANNEL_COUNT,
			samples_to_copy * SOUND_CHANNEL_COUNT * sizeof(float));
		frame_count -= samples_to_copy;
		memcpy(output, game.sound_state.buffer, 
			min(write_index, (int)frame_count) * SOUND_CHANNEL_COUNT * sizeof(float));

		game.sound_state.read_index = (read_index + samples_to_copy
			+ min(write_index, (int)frame_count)) % game.sound_state.sample_count;
	}

	// for (int i = 0; i < frame_count; i++)
    // {

    //     float *out0 = (float *)output;
    //     output = (float *)output + 1;
    //     float *out1 = (float *)output;
    //     output = (float *)output + 1;

	// 	//*out0 = 0;
	// 	//*out1 = 0;
    //     // if (sample >= g_sound_file.sampleCount*2)
    //     //     sample = 0;
		
    //     // float value0 = g_sound_file.samples[sample++];
    //     // float value1 = g_sound_file.samples[sample++];
	// 	// float value = sinf(((float)sample / SOUND_SAMPLE_RATE) * 2 * 3.14159265359f * 300);
    //     // *out0 = value;
    //     // *out1 = value;
	// 	// sample++;
    // }
}

void play_sound(Game &game, LoadedSound &loaded_sound, entity_id entity = 0)
{
	SoundPlaying *sound = (SoundPlaying *)arena_alloc_zero(&game.arena, sizeof(SoundPlaying));
	sound->sound = &loaded_sound;
	sound->entity = entity;

	sound->next = game.first_playing_sound;
	if (game.first_playing_sound)
		game.first_playing_sound->prev = sound;
	game.first_playing_sound = sound;
}

void update_sound(Game &game, World &world)
{
	if (!game.first_playing_sound)
		return ;

	SoundState &state = game.sound_state;

	// TODO: cleanup
	// TODO: why setting frames_to_write > 1 doesn't work
	int frames_to_write = 1;
	int fps = 60;
	int max_samples_to_write =  ((SOUND_SAMPLE_RATE * frames_to_write) / fps);


	int write_index = state.write_index;
	int read_index = state.read_index;
	//printf("write %d %d %d\n", read_index, write_index, state.sample_count);

	int can_write = 0;
	if (read_index <= write_index)
		can_write = read_index + state.sample_count - write_index; 
	else
		can_write = read_index - write_index;
	
	can_write -= 1;

	max_samples_to_write = min(max_samples_to_write, can_write);

	if (max_samples_to_write <= 0)
		return ;

	Entity *player = get_entity(world, world.player_id);
	v3 player_forward = normalize(V3(cosf(world.player_camera_rotation.z), sinf(world.player_camera_rotation.z), 0));
	v3 player_up = WORLD_UP;
	v3 player_right = normalize(cross(player_forward, player_up));
		
	int index = write_index;
	for (int sample = 0; sample < max_samples_to_write; sample++) {
		for (int i = 0; i < SOUND_CHANNEL_COUNT; i++)
			state.buffer[index * SOUND_CHANNEL_COUNT + i] = 0;
		index++;
		if (index == state.sample_count)
			index = 0;
	}
	//printf("INIT JUMP SOUND %d %d\n", max_samples_to_write, game.first_playing_sound->sound->sample_count);

	for (SoundPlaying *playing_sound = game.first_playing_sound; 
		playing_sound;) {

		Entity *e = get_entity(world, playing_sound->entity);
		float volume[2] = {1, 1};
		if (e) {
			v3 to_e = normalize(e->position - world.player_camera_p);
			float x = dot(to_e, player_right);
			float y = dot(to_e, player_forward);
			float a = fabsf(atan2(y, x));
			a = a / PI;
			volume[0] = a;
			volume[1] = 1 - a;

			float dist = 1 - logf(length(e->position - world.player_camera_p)) / 5;
			if (dist < 0)
				dist = 0;
			volume[0] *= dist;
			volume[1] *= dist;
		}
		
		int samples_to_write = min(max_samples_to_write, 
		playing_sound->sound->sample_count - playing_sound->samples_played);

		index = write_index;
		for (int sample = 0; sample < samples_to_write; sample++) {
			for (int i = 0; i < SOUND_CHANNEL_COUNT; i++) {
				state.buffer[index * SOUND_CHANNEL_COUNT + i] += 
					playing_sound->sound->samples[playing_sound->samples_played
						* SOUND_CHANNEL_COUNT + i] * volume[i] * game.master_volume;
			}
			playing_sound->samples_played++;
			index++;
			if (index == state.sample_count)
				index = 0;
		}
		SoundPlaying *next = playing_sound->next;
		if (playing_sound->samples_played == playing_sound->sound->sample_count) {
			if (next)
				next->prev = playing_sound->prev;
			if (playing_sound->prev)
				playing_sound->prev->next = next;
			if (game.first_playing_sound == playing_sound)
				game.first_playing_sound = next;
		}
		playing_sound = next;
	}
	//printf("finished? %d\n", game.first_playing_sound == 0);
	//printf("END JUMP SOUND\n");
	int new_write_index = (write_index + max_samples_to_write) % state.sample_count;
	state.write_index = new_write_index;
}
