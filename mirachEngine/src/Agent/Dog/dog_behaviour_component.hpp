#pragma once
#include <Agent/Dog/dog.hpp>
namespace Dog 
{
	/*
	*  Component which handles dog behaviour.
	*  Note: SetEntity must be set for it to behave properly.
	*/
	class DogBehaviour 
	{
	public:
		DogBehaviour();

		void Init(std::shared_ptr<MurdochScene> scene, Entity dog);
		State GetState();
		/*
		 * Should be called every frame. Make use of the different methods below.
		 * Can do
		 */
		void Update(float deltaTime);
		void UpdateImage();

		friend class Debug;

	private:
		// Bunch of different actions potentially?
		void Think();
		void Sit(float dt);
		void Walk(float dt);
		void Eat(float dt);
		void Play(float dt);
		void Rest(float dt);
		void Clean(float dt);

		float m_timeSinceWait;
		float m_timeToWait;
		float m_nextTimeToWait;

		// State of the dog.
		State m_nextState;
		State m_state;
		State m_prevState;

		Vector3f m_toPos;
		Vector3f m_fromPos;
		Vector3f m_minPos;
		Vector3f m_maxPos;
		Vector3f m_currentPos;

		// Which dog this component belongs to.
		Entity m_dog;
		// Which scene this dog belongs to.
		std::shared_ptr<MurdochScene> m_scene;
	};

}