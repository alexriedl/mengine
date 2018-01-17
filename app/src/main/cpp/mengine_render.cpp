

#define PushRenderElement(Group, type) (type *)PushRenderElement_(Group, sizeof(type), RenderGroupEntryType_##type)
inline void *PushRenderElement_(render_group *Group, u32 Size, render_group_entry_type Type)
{
	void *Result = 0;

	Size += sizeof(render_group_entry_header);

	if((Group->PushBufferSize + Size) < Group->MaxPushBufferSize)
	{
		render_group_entry_header *Header == (render_group_entry_header *)(Group->PushBufferBase + Group->PushBufferSize);
		Header->Type = Type;
		Result = (u8 *)Header + sizeof(*Header);
		Group->PushBufferSize += Size;
	}
	else
	{
		InvalidCodePath;
	}

	return Result;
}

inline void PushRectangle(render_group *Group, u32 Color, r32 X, r32 Y, r32 Width, r32 Height)
{
	render_entry_rectangle *Entry = PushRenderElement(Group, render_entry_rectangle);
	if(Entry)
	{
		Entry->X = X;
		Entry->Y = Y;
		Entry->Width = Width;
		Entry->Height = Height;
	}
}

inline void PushTexturedRectangle(render_group *Group, u32 Texture, u32 Color, r32 X, r32 Y, r32 Width, r32 Height)
{
	render_entry_textured_rectangle *Entry = PushRenderElement(Group, render_entry_textured_rectangle);
	if(Entry)
	{
		Entry->Texture = Texture;
		Entry->X = X;
		Entry->Y = Y;
		Entry->Width = Width;
		Entry->Height = Height;
	}
}
