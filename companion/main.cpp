#include <vitasdk.h>
#include <vitaGL.h>
#include <imgui_vita.h>
#include <stdio.h>

#define CONFIG_FILE_PATH "ux0:data/gtasa/config.txt"
#define NUM_OPTIONS 15

int _newlib_heap_size_user = 192 * 1024 * 1024;

#define SKYGFX_COLOR_FILTER_NUM 4
enum SkyGfxColorFilter {
	SKYGFX_COLOR_FILTER_MOBILE,
	SKYGFX_COLOR_FILTER_NONE,
	SKYGFX_COLOR_FILTER_PS2,
	SKYGFX_COLOR_FILTER_PC,
};

char *SkyGfxColorFilterName[SKYGFX_COLOR_FILTER_NUM] = {
	"Mobile",
	"None",
	"PS2",
	"PC"
};

int touch_x_margin = 100;
int use_fios2 = GL_TRUE;
int io_cache_block_num = 64;
int io_cache_block_size = 65536;
int fix_heli_plane_camera = GL_TRUE;
int fix_skin_weights = GL_TRUE;
int fix_map_bottleneck = GL_TRUE;
int use_shader_cache = GL_TRUE;
int skygfx_ps2_shading = GL_TRUE;
int skygfx_colorfilter = SKYGFX_COLOR_FILTER_PS2;
int skygfx_ps2_sun = GL_TRUE;
int disable_detail_textures = GL_TRUE;
int disable_ped_spec = GL_TRUE;
int disable_tex_bias = GL_TRUE;
int disable_alpha_testing = GL_FALSE;

void loadConfig() {
	char buffer[30];
	int value;
	int gTempPostProcessing = 0;
	int gTempOverlay = 0;
	
	FILE *config = fopen(CONFIG_FILE_PATH, "r");

	if (config)
	{
		while (EOF != fscanf(config, "%[^ ] %d\n", buffer, &value))
		{
			if (strcmp("use_fios2", buffer) == 0) use_fios2 = value;
			else if (strcmp("io_cache_block_num", buffer) == 0) io_cache_block_num = value;
			else if (strcmp("io_cache_block_size", buffer) == 0) io_cache_block_size = value;
			
			else if (strcmp("touch_x_margin", buffer) == 0) touch_x_margin = value;
			else if (strcmp("fix_heli_plane_camera", buffer) == 0) fix_heli_plane_camera = value;
			
			else if (strcmp("fix_skin_weights", buffer) == 0) fix_skin_weights = value;
			else if (strcmp("fix_map_bottleneck", buffer) == 0) fix_map_bottleneck = value;
			else if (strcmp("use_shader_cache", buffer) == 0) use_shader_cache = value;
			
			else if (strcmp("skygfx_ps2_shading", buffer) == 0) skygfx_ps2_shading = value;
			else if (strcmp("skygfx_colorfilter", buffer) == 0) skygfx_colorfilter = value;
			else if (strcmp("skygfx_ps2_sun", buffer) == 0) skygfx_ps2_sun = value;

			else if (strcmp("disable_detail_textures", buffer) == 0) disable_detail_textures = value;
			else if (strcmp("disable_ped_spec", buffer) == 0) disable_ped_spec = value;
			else if (strcmp("disable_tex_bias", buffer) == 0) disable_tex_bias = value;
			else if (strcmp("disable_alpha_testing", buffer) == 0) disable_alpha_testing = value;
		}
		fclose(config);
	}
}

void saveConfig()
{
	FILE *config = fopen(CONFIG_FILE_PATH, "w+");
	
	if (config) {
		fprintf(config, "%s %d\n", "use_fios2", use_fios2);
		fprintf(config, "%s %d\n", "io_cache_block_num", io_cache_block_num);
		fprintf(config, "%s %d\n", "io_cache_block_size", io_cache_block_size);
		
		fprintf(config, "%s %d\n", "touch_x_margin", touch_x_margin);
		fprintf(config, "%s %d\n", "fix_heli_plane_camera", fix_heli_plane_camera);
		
		fprintf(config, "%s %d\n", "fix_skin_weights", fix_skin_weights);
		fprintf(config, "%s %d\n", "fix_map_bottleneck", fix_map_bottleneck);
		fprintf(config, "%s %d\n", "use_shader_cache", use_shader_cache);
		
		fprintf(config, "%s %d\n", "skygfx_ps2_shading", skygfx_ps2_shading);
		fprintf(config, "%s %d\n", "skygfx_colorfilter", skygfx_colorfilter);
		fprintf(config, "%s %d\n", "skygfx_ps2_sun", skygfx_ps2_sun);
		
		fprintf(config, "%s %d\n", "disable_detail_textures", disable_detail_textures);
		fprintf(config, "%s %d\n", "disable_ped_spec", disable_ped_spec);
		
		fprintf(config, "%s %d\n", "disable_tex_bias", disable_tex_bias);
		fprintf(config, "%s %d\n", "disable_alpha_testing", disable_alpha_testing);
		fclose(config);
	}
}

char *options_descs[NUM_OPTIONS] = {
	"Enables sceFios2 usage for I/O operations. This allows to have caching on input operations reducing stuttering while in game.\n\nThe default value is: Enabled.", // use_fios2
	"Number of blocks to use with sceFios2.\n\nThe default value is: 64.", // io_cache_block_num
	"Size in bytes for each block used by sceFios2.\n\nThe default value is: 65536.", // io_cache_block_size
	"Deadzone in pixels to use between inputs on both rearpad and touchscreen.\n\nThe default value is: 100.", // touch_x_margin
	"Makes it possible to move the camera with the right stick when using a flying vehicle (Planes and helicopters).\n\nThe default value is: Enabled.", // fix_heli_plane_camera
	"Select the desired post processing effect filter to apply to the 3D rendering.\n\nThe default value is: PS2.", // skygfx_colorfilter
	"Enables shading effects that resamble the PS2 build.\n\nThe default value is: Enabled.", // skygfx_ps2_shading
	"Enables corona sun effect that resambles the PS2 build.\n\nThe default value is: Enabled.", // skygfx_ps2_sun
	"When enabled, detail textures will be rendered.\n\nThe default value is: Disabled.", // disable_detail_textures
	"When enabled, mipmaps will have more precise bias adjustments at the cost of more expensive GPU code execution.\n\nThe default value is: Disabled.", // disable_tex_bias
	"When enabled, alpha testing is executed during GPU code execution. Makes GPU code more expensive but solves several glitches in the game related to translucent models.\n\nThe default value is: Enabled.", // disable_alpha_testing
	"Makes hardware accelerated skinning properly work. Fixes broken animations especially noticeable in facial animations.\n\nThe default value is: Enabled.", // fix_skin_weights
	"When enabled, peds will have specular lighting reflections applied to their models.\n\nThe default value is: Disabled.", // disable_ped_spec
	"Makes compiled shaders be cached on storage for subsequent usage. When enabled, the game will stutter on very first time a shader is compiled but will make the game have more fluid gameplay later.\n\nThe default value is: Enabled.", // use_shader_cache
	"Removes regions highlighting in the pause menu map. This will fix the performance issues during advanced stages of the game in pause menu.\n\nThe default value is: Enabled." // fix_map_bottleneck
};

char *desc = nullptr;

void SetDescription(int i) {
	if (ImGui::IsItemHovered())
		desc = options_descs[i];
}

int main(){
	loadConfig();
	int exit_code = 0xDEAD;
	
	vglInitExtended(0, 960, 544, 0x1800000, SCE_GXM_MULTISAMPLE_4X);
	ImGui::CreateContext();
	ImGui_ImplVitaGL_Init();
	ImGui_ImplVitaGL_TouchUsage(false);
	ImGui_ImplVitaGL_GamepadUsage(true);
	ImGui::StyleColorsDark();
	ImGui::GetIO().MouseDrawCursor = false;
	
	while (exit_code == 0xDEAD) {
		desc = nullptr;
		ImGui_ImplVitaGL_NewFrame();
		
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiSetCond_Always);
		ImGui::SetNextWindowSize(ImVec2(960, 544), ImGuiSetCond_Always);
		ImGui::Begin("Grand Theft Auto: San Andreas - Config Manager", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus);
		
		ImGui::TextColored(ImVec4(255, 255, 0, 255), "I/O Caching");
		ImGui::Text("Use sceFios2: "); ImGui::SameLine();
		if (ImGui::SmallButton(use_fios2 ? "Enabled##0" : "Disabled##0"))
			use_fios2 = !use_fios2;
		SetDescription(0);
		if (use_fios2) {
			ImGui::Text("Cache Blocks Amount:"); ImGui::SameLine();
			ImGui::SliderInt("##io_cache_block_num", &io_cache_block_num, 2, 256);
			SetDescription(1);
			ImGui::Text("Cache Blocks Size:"); ImGui::SameLine();
			ImGui::DragInt("##io_cache_block_size", &io_cache_block_size, 1024.0f, 1024, 1024 * 256);
			SetDescription(2);
		} else {
			ImGui::Spacing();
			ImGui::Spacing();
		}
		ImGui::Separator();
		
		ImGui::TextColored(ImVec4(255, 255, 0, 255), "Inputs");
		ImGui::Text("Touchpanels Deadzone:"); ImGui::SameLine();
		ImGui::SliderInt("##touch_x_margin", &touch_x_margin, 0, 256);
		SetDescription(3);
		ImGui::Text("Flying Vehicles Camera Fix:"); ImGui::SameLine();
		if (ImGui::SmallButton(fix_heli_plane_camera ? "Enabled##1" : "Disabled##1"))
			fix_heli_plane_camera = !fix_heli_plane_camera;
		SetDescription(4);
		ImGui::Separator();
		
		ImGui::TextColored(ImVec4(255, 255, 0, 255), "Skygfx by aap");
		ImGui::Text("PostFX Colour Filter:"); ImGui::SameLine();
		if (ImGui::SmallButton(SkyGfxColorFilterName[skygfx_colorfilter]))
			skygfx_colorfilter = (skygfx_colorfilter + 1) % SKYGFX_COLOR_FILTER_NUM;
		SetDescription(5);
		ImGui::Text("PS2 Shading:"); ImGui::SameLine();
		if (ImGui::SmallButton(skygfx_ps2_shading ? "Enabled##2" : "Disabled##2"))
			skygfx_ps2_shading = !skygfx_ps2_shading;
		SetDescription(6);
		ImGui::Text("PS2 Corona Sun:"); ImGui::SameLine();
		if (ImGui::SmallButton(skygfx_ps2_sun ? "Enabled##10" : "Disabled##10"))
			skygfx_ps2_sun = !skygfx_ps2_sun;
		SetDescription(7);
		ImGui::Separator();
		
		ImGui::TextColored(ImVec4(255, 255, 0, 255), "Graphics");
		ImGui::Text("Detail Textures:"); ImGui::SameLine();
		if (ImGui::SmallButton(disable_detail_textures ? "Disabled##3" : "Enabled##3"))
			disable_detail_textures = !disable_detail_textures;
		SetDescription(8);
		ImGui::Text("Texture Bias:"); ImGui::SameLine();
		if (ImGui::SmallButton(disable_tex_bias ? "Disabled##4" : "Enabled##4"))
			disable_tex_bias = !disable_tex_bias;
		SetDescription(9);
		ImGui::Text("Alpha Testing:"); ImGui::SameLine();
		if (ImGui::SmallButton(disable_alpha_testing ? "Disabled##5" : "Enabled##5"))
			disable_alpha_testing = !disable_alpha_testing;
		SetDescription(10);
		ImGui::Text("Skinning Fix:"); ImGui::SameLine();
		if (ImGui::SmallButton(fix_skin_weights ? "Enabled##6" : "Disabled##6"))
			fix_skin_weights = !fix_skin_weights;
		SetDescription(11);
		ImGui::Text("Peds Reflections:"); ImGui::SameLine();
		if (ImGui::SmallButton(disable_ped_spec ? "Disabled##7" : "Enabled##7"))
			disable_ped_spec = !disable_ped_spec;
		SetDescription(12);
		ImGui::Separator();
		
		ImGui::TextColored(ImVec4(255, 255, 0, 255), "Optimizations");
		ImGui::Text("Shader Cache:"); ImGui::SameLine();
		if (ImGui::SmallButton(use_shader_cache ? "Enabled##8" : "Disabled##8"))
			use_shader_cache = !use_shader_cache;
		SetDescription(13);
		ImGui::Text("Map Bottleneck Fix:"); ImGui::SameLine();
		if (ImGui::SmallButton(fix_map_bottleneck ? "Enabled##9" : "Disabled##9"))
			fix_map_bottleneck = !fix_map_bottleneck;
		SetDescription(14);
		ImGui::Separator();
		
		if (ImGui::Button("Save and Exit"))
			exit_code = 0;
		ImGui::SameLine();
		if (ImGui::Button("Save and Launch the game"))
			exit_code = 1;
		ImGui::SameLine();
		if (ImGui::Button("Discard and Exit"))
			exit_code = 2;
		ImGui::SameLine();
		if (ImGui::Button("Discard and Launch the game"))
			exit_code = 3;
		ImGui::Separator();
		
		if (desc) {
			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::TextWrapped(desc);
		}
		
		ImGui::End();
		glViewport(0, 0, static_cast<int>(ImGui::GetIO().DisplaySize.x), static_cast<int>(ImGui::GetIO().DisplaySize.y));
		ImGui::Render();
		ImGui_ImplVitaGL_RenderDrawData(ImGui::GetDrawData());
		vglSwapBuffers(GL_FALSE);
	}
	
	if (exit_code < 2) // Save
		saveConfig();
	
	if (exit_code % 2 == 1) // Launch
		sceAppMgrLoadExec("app0:/eboot.bin", NULL, NULL);
	
	return 0;
}