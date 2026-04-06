#include "animation_system.hpp"

void AnimationSystem::Update(EntityManager& em, float deltaTime) 
{
    // Iterate over all entities that have an MD2Animator component
    for (auto& [entity, anim] : em.GetAllComponents<MD2Animator>()) 
    {
        if (!anim.mesh)
        {
            continue; // Skip if mesh is not assigned
        }

        // Check if animation name changed, and reset animation state
        if (anim.currentAnim != anim.lastAppliedAnim) 
        {
            anim.mesh->SetAnimation(anim.currentAnim); // Notify mesh of new animation
            anim.animTime = 0.0f;                       // Reset animation timer
            anim.currentFrame = anim.mesh->GetAnimationRange(anim.currentAnim).startFrame;
            anim.nextFrame = anim.currentFrame + 1;
            anim.lastAppliedAnim = anim.currentAnim;
        }

        const AnimationRange& range = anim.mesh->GetAnimationRange(anim.currentAnim);
        float frameDuration = 1.0f / 3.0f; // Duration per frame (e.g., 3 FPS)

        anim.animTime += deltaTime; // Accumulate time

        // Advance frames based on elapsed time
        while (anim.animTime >= frameDuration) 
        {
            anim.animTime -= frameDuration;
            anim.currentFrame = anim.nextFrame;
            anim.nextFrame++;

            // Loop animation if end of range is reached
            if (anim.nextFrame > range.endFrame)
            {
                anim.nextFrame = range.startFrame;
            }
        }

        // Interpolation factor between currentFrame and nextFrame
        float alpha = anim.animTime / frameDuration;

        // Compute interpolated vertex positions between current and next frame
        anim.mesh->Interpolate(anim.currentFrame, anim.nextFrame, alpha, anim.interpolatedVerts);

        // Upload interpolated vertex data to the entity's animated VBO
        Graphics::BindBuffer(BufferTarget::ArrayBuffer, anim.animatedVBO);
        Graphics::BufferSubData(BufferTarget::ArrayBuffer, 0,
            sizeof(Vector3f) * anim.interpolatedVerts.size(),
            anim.interpolatedVerts.data());
    }
}
