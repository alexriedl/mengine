static void checkGlError(const char *operation)
{
	GLint error;
	for(error = glGetError(); error; error = glGetError())
	{
		LOGE("after %s() glError (0x%x)", operation, error);
	}
}

internal void OpenGLRenderCommands(game_render_commands *Commands, u32 WindowWidth, u32 WindowHeight)
{
	game_render_settings Settings = Commands->Settings;
	u32 RenderWidth = Settings.Width;
	u32 RenderHeight = Settings.Height;

	{
		glDepthMask(GL_TRUE);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glDepthFunc(GL_LEQUAL);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
		glEnable(GL_SCISSOR_TEST);
		glDisable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

		checkGlError("OpenGLRenderCommands::GLInitialization");
	}

	glViewport(0, 0, WindowWidth, WindowHeight);
	glScissor(0, 0, WindowWidth, WindowHeight);
	glClearColor(1, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	checkGlError("OpenGLRenderCommands::ClearScreen");

	glViewport(0, 0, RenderWidth, RenderHeight);
	glScissor(0, 0, RenderWidth, RenderHeight);
	glClearColor(1, 1, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	checkGlError("OpenGLRenderCommands::ScissorClearScreen");
}
