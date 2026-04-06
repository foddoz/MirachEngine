#pragma once
#include <string>
#include <Math/math.hpp>
#include <Program/murdoch_scene.hpp>
#include <ECS/ecs_types.hpp>
#include <ScriptingAPI/scripting_murdoch_scene.hpp>
#include "Graphics/Rendering/image_renderer.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "Emotion/emotion_system.hpp"
//#include "../../Physics/physics_system.hpp"

namespace Dog 
{
	inline ImageRenderer renderer;

	struct DogImage 
	{
		std::string name;
		std::shared_ptr<Texture> texture;   
		bool faceToPlayer{ true };

		DogImage() 
			: 
			texture(std::make_shared<Texture>(TextureTarget::Texture2D)) 
		{

		}

		explicit DogImage(std::string n)
			: name(std::move(n)), texture(std::make_shared<Texture>(TextureTarget::Texture2D)) 
		{
		}
	};

	struct DogEmojiImage 
	{
		Dog::DogImage emojiImage;        
		Emotion emotion; 

		DogEmojiImage()
			:
			emojiImage(),
			emotion(Emotion::Happy)
		{

		}
	};

	enum class State 
	{
		Think,
		Sit,
		Walk,
		Rest,
		Eat,
		Play,
		Clean
	};

	enum Breed 
	{
		GermanShepherd,
		GoldenRetriever,
		ToyPoodle
	};
	
	// Spawn a dog with the given parameters.
	Entity SpawnDog(std::shared_ptr<MurdochScene> scene, const Vector3f&  pos, Breed breed = GermanShepherd);

	// Add global functions to Lua.
	void AddToScriptingAPI(MurdochScripting& API);

	//Get Dog Breed/State/Emotion Name
	std::string ToString(Breed breed);
	std::string ToString(State state);
	std::string ToString(Emotion emotion);

	//Initialize renderer
	bool InitRenderer();

	//Update texture
	void ChangeImage(EntityManager& ecs, Entity dog, State state);

	void ChangeEmojiImage(DogImage& EmojiImage);

	//Draw Dog image
	void RenderDogImage(DogImage& image, const Transform& transform, const float* view,
		const float* proj);

	void RenderDogEmoji(DogImage& emoji, const Transform& transform, const float* view,
		const float* proj, Vector2f dogSize, Vector2f emojiSize, float yOffset, float gap);
};