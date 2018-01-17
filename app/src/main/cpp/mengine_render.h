#if !defined(MENGINE_RENDER_H)

enum render_group_entry_type
{
	RenderGroupEntryType_render_entry_textured_rectangle,
	RenderGroupEntryType_Rectangle,
};

struct render_group_entry_header
{
	render_group_entry_type Type;
};

struct render_entry_rectangle
{
	u32 Color;
	r32 X;
	r32 Y;
	r32 Width;
	r32 Height;
};

struct render_entry_textured_rectangle
{
	u32 Texture;
	u32 Color;
	r32 X;
	r32 Y;
	r32 Width;
	r32 Height;
};

struct render_group
{
	u32 MaxPushBufferSize;
	u32 PushBufferSize;
	u8 *PushBufferBase;
}

#define MENGINE_RENDER_H
#endif
