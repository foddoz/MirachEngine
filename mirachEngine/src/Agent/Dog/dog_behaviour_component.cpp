#include "dog_behaviour_component.hpp"
#include "Wellness/wellness.hpp"

#include "Physics/physics.hpp"
using namespace Dog;

DogBehaviour::DogBehaviour()
	:
	m_timeSinceWait(0.f),
	m_timeToWait(0.f),
	m_nextTimeToWait(0.f),
	m_toPos(Vector3f()),
	m_fromPos(Vector3f()),
	m_minPos(Vector3f(476.0f, 3.0f, 500.0f)),
	m_maxPos(Vector3f(529.0f, 3.0f, 520.0f)),
	m_currentPos(Vector3f()),
	m_nextState(State::Think),
	m_state(State::Think),
	m_prevState(State::Think)
{

}

void DogBehaviour::Init(std::shared_ptr<MurdochScene> scene, Entity dog)
{
	m_scene = scene;
	m_dog = dog;
	m_state = State::Think;
	m_prevState = State::Think;
	m_nextState = State::Think;
	Transform& transform = m_scene->GetECS().GetComponent<Transform>(m_dog);
	m_toPos = transform.GetPosition();
	m_fromPos = transform.GetPosition();
	m_currentPos = transform.GetPosition();
}

Dog::State Dog::DogBehaviour::GetState()
{
	return m_state;
}

void Dog::DogBehaviour::Update(float deltaTime)
{
	EntityManager& ECS = m_scene->GetECS();
	Wellness& wellness = ECS.GetComponent<Wellness>(m_dog);

	wellness.Update(deltaTime);

	m_timeSinceWait += deltaTime;

	switch (m_state) 
	{
		case State::Think:
			Think();
			break;
		case State::Sit:
			Sit(deltaTime);
			break;
		case State::Walk:
			Walk(deltaTime);
			break;
		case State::Rest:
			Rest(deltaTime);
			break;
		case State::Eat:
			Eat(deltaTime);
			break;
		case State::Play:
			Play(deltaTime);
			break;
		case State::Clean:
			Clean(deltaTime);
			break;
		default:
			Think();
	}

	if ((m_prevState != m_state) && (m_state != State::Think)) 
	{
		UpdateImage();
	}

	m_prevState = m_state;
	Physics::RigidBody*& body = m_scene->GetECS().GetComponent<Physics::RigidBody*>(m_dog);
	body->SetPosition(m_currentPos);
}

void Dog::DogBehaviour::UpdateImage()
{
	EntityManager& ECS = m_scene->GetECS();

	if (!ECS.HasComponent<DogImage>(m_dog)) 
	{
		return;
	}

	DogImage& image = ECS.GetComponent<DogImage>(m_dog);

	ChangeImage(ECS, m_dog, m_state);
}

void Dog::DogBehaviour::Think()
{
	m_timeSinceWait = 0.0f;
	m_timeToWait = (float) rand() / (float) RAND_MAX;

	m_timeToWait *= 4;
	m_timeToWait += 3;

	float randx = (float)rand() / (float)RAND_MAX;
	float randy = (float)rand() / (float)RAND_MAX;
	float randz = (float)rand() / (float)RAND_MAX;

	m_toPos.x = m_minPos.x + (m_maxPos.x - m_minPos.x) * randx;
	m_toPos.y = m_minPos.y + (m_maxPos.y - m_minPos.y) * randy;
	m_toPos.z = m_minPos.z + (m_maxPos.z - m_minPos.z) * randz;

	Physics::RigidBody*& body = m_scene->GetECS().GetComponent<Physics::RigidBody*>(m_dog);
	m_fromPos = body->GetPosition();

	Wellness& wellness = m_scene->GetECS().GetComponent<Wellness>(m_dog);
	Wellness::Component low = wellness.GetLowest();

	EmotionModel& emo = m_scene->GetECS().GetComponent<EmotionModel>(m_dog);
	DogEmojiImage& emoji = m_scene->GetECS().GetComponent<DogEmojiImage>(m_dog);

	if (wellness.GetValue(low) >= 0.8) 
	{
		emo.emotion = Emotion::Happy;
		emo.pad.pleasure = 1.0f;
		emo.pad.arousal = 1.0f;
		emo.pad.dominance = 1.0f;
		emoji.emotion = Emotion::Happy;
		emoji.emojiImage.name = "happy";
		ChangeEmojiImage(emoji.emojiImage);
		m_state = State::Sit;
		m_nextState = State::Think;
		return;
	}
	else 
	{
		emo.emotion = Emotion::Sad;
		emoji.emotion = Emotion::Sad;
		emoji.emojiImage.name = "sad";
		ChangeEmojiImage(emoji.emojiImage);
		emo.pad.pleasure = 0.2f;
		emo.pad.arousal = 0.2f;
		emo.pad.dominance = 0.2f;
	}
	switch (low) 
	{
	case Wellness::Component::Cleanliness:
		m_nextTimeToWait = 5.0f;
		m_state = State::Walk;
		m_nextState = State::Clean;
		break;
	case Wellness::Component::Happiness:
		m_nextTimeToWait = 5.0f;
		m_state = State::Walk;
		m_nextState = State::Play;
		break;
	case Wellness::Component::Food:
		m_nextTimeToWait = 5.0f;
		m_state = State::Walk;
		m_nextState = State::Eat;
		break;
	case Wellness::Component::Water:
		m_nextTimeToWait = 5.0f;
		m_state = State::Walk;
		m_nextState = State::Eat;
		break;
	case Wellness::Component::Sleep:
		m_nextTimeToWait = 5.0f;
		m_state = State::Walk;
		m_nextState = State::Rest;
		break;
	}
}

void Dog::DogBehaviour::Sit(float dt)
{
	if (m_timeSinceWait >= m_timeToWait) 
	{
		m_state = State::Think;
	}
}

void Dog::DogBehaviour::Walk(float dt)
{
	float delta = m_timeSinceWait / m_timeToWait;

	if (m_timeSinceWait >= m_timeToWait) 
	{
		m_timeSinceWait = 0;
		m_timeToWait = m_nextTimeToWait;
		m_state = m_nextState;
		m_nextState = State::Think;
		m_currentPos = m_toPos;
	}
	else 
	{
		m_currentPos.x = m_fromPos.x + (m_toPos.x - m_fromPos.x) * delta;
		m_currentPos.y = m_fromPos.y + (m_toPos.y - m_fromPos.y) * delta;
		m_currentPos.z = m_fromPos.z + (m_toPos.z - m_fromPos.z) * delta;
	}
}

void Dog::DogBehaviour::Eat(float dt)
{
	float ratePerSec = 0.025f;
	Wellness& wellness = m_scene->GetECS().GetComponent<Wellness>(m_dog);
	wellness.AddValue(Wellness::Component::Food, ratePerSec * dt);
	wellness.AddValue(Wellness::Component::Water, ratePerSec * dt);
	if (m_timeSinceWait >= m_timeToWait) 
	{
		m_state = State::Think;
	}
}

void Dog::DogBehaviour::Play(float dt)
{
	float ratePerSec = 0.05f;
	Wellness& wellness = m_scene->GetECS().GetComponent<Wellness>(m_dog);
	wellness.AddValue(Wellness::Component::Happiness, ratePerSec * dt);
	if (m_timeSinceWait >= m_timeToWait) 
	{
		m_state = State::Think;
	}
}

void Dog::DogBehaviour::Rest(float dt)
{
	float ratePerSec = 0.05f;
	Wellness& wellness = m_scene->GetECS().GetComponent<Wellness>(m_dog);
	wellness.AddValue(Wellness::Component::Sleep, ratePerSec * dt);
	if (m_timeSinceWait >= m_timeToWait) 
	{
		m_state = State::Think;
	}
}

void Dog::DogBehaviour::Clean(float dt)
{
	float ratePerSec = 0.05f;
	Wellness& wellness = m_scene->GetECS().GetComponent<Wellness>(m_dog);
	wellness.AddValue(Wellness::Component::Cleanliness, ratePerSec * dt);
	if (m_timeSinceWait >= m_timeToWait) 
	{
		m_state = State::Think;
	}
}
