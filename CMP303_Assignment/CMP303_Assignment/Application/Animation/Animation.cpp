#include "Animation.h"

void Animation::AddFrame(sf::IntRect rect)
{
	mFrames.push_back(rect);
}

void Animation::Update(float dt)
{
	if (mIsPlaying)
	{
		mElapsedTime += dt;
		if (mElapsedTime >= mAnimationSpeed)
		{
			mCurrentFrame++;
			if (mCurrentFrame >= (int)mFrames.size())
			{
				if (mIsLooping)
				{
					mCurrentFrame = 0;
				}
				else
				{
					mCurrentFrame--;
					mIsPlaying = false;
				}
			}
			mElapsedTime = 0.0f;
		}
	}
}

void Animation::Reset()
{
	mElapsedTime = 0.0f;
	mCurrentFrame = 0.0f;
}

sf::IntRect& Animation::CurrentFrame()
{
	mFrame = mFrames[mCurrentFrame];
	if (mFlipped)
	{
		mFrame = sf::IntRect(mFrame.left + mFrame.width, mFrame.top, -mFrame.width, mFrame.height);
	}
}

inline void Animation::Stop()
{
	mCurrentFrame = 0;
	mElapsedTime = 0.0f;
	mIsPlaying = false;
}
