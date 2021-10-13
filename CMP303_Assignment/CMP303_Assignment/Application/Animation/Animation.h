#pragma once
#include <vector>
#include <SFML/Graphics/Rect.hpp>

//@author Dr Paul Robertson.
//@note This class has been taken from the the CMP105 module with little modification
//aside from marking functions inline and const (i.e Getter() const {...})

class Animation
{
public:
	Animation() = default;
	Animation(const Animation& other) = default;

	// Create animation
	void AddFrame(sf::IntRect rect);
	inline uint32_t NumberOfFrames() const { return (uint32_t)mFrames.size(); }

	// update/reset animation
	void Update(float dt);
	void Reset();

	// get current frame of animation
	sf::IntRect& CurrentFrame();

	// Animation controls
	inline void SetFrameSpeed(float speed) { mAnimationSpeed = speed; }
	float FrameSpeed() const { return mAnimationSpeed; };
	inline void SetFlipped(bool flip) { mFlipped = flip; };
	bool IsFlipped() const { return mFlipped; };
	inline void LoopAnimation(bool loop) { mIsLooping = loop; };
	bool IsLooping() const { return mIsLooping; };
	inline void PlayAnimation(bool play) { mIsPlaying = play; };
	bool IsPlaying() const { return mIsPlaying; };
	inline void SetAnimationPlay(bool play) { mIsPlaying = true; };
	inline void Stop();
	inline void Pause() { mIsPlaying = false; };

protected:
	std::vector<sf::IntRect> mFrames;
	sf::IntRect mFrame;
	int mCurrentFrame;
	float mAnimationSpeed;
	bool mFlipped;
	bool mIsLooping;
	bool mIsPlaying;
	float mElapsedTime;

};
