#ifndef _ANIMATION_H_
#define _ANIMATION_H_

#include "SDL\include\SDL_rect.h"

#define MAX_FRAMES 25

class Animation
{
private:
	float current_frame;
	int last_frame = 0;

public:
	float speed = 1.0f;
	SDL_Rect frames[MAX_FRAMES];

public:
	void PushBack(const SDL_Rect& rect)
	{
		frames[last_frame++] = rect;
	}
	SDL_Rect& GetCurrentFrame()
	{
		current_frame += speed;
		if (current_frame >= last_frame)
		{
			current_frame = 0;
		}
		return frames[(int)current_frame];
	}


};



#endif
