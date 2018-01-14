#if !defined(MENGINE_SHARED_H)

internal void CatStrings(size_t SourceACount, char *SourceA, size_t SourceBCount, char *SourceB, size_t DestCount,
                         char *Dest)
{
	for(int Index = 0; Index < SourceACount; Index++)
	{
		*Dest++ = *SourceA++;
	}

	for(int Index = 0; Index < SourceBCount; Index++)
	{
		*Dest++ = *SourceB++;
	}

	*Dest++ = 0;
}

internal int StringLength(char *Str)
{
	int Count = 0;
	while(*Str++)
	{
		++Count;
	}

	return Count;
}

#define MENGINE_SHARED_H
#endif
