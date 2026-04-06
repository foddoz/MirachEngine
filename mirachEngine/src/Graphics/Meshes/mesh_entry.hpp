#ifndef MESH_ENTRY_HPP
#define MESH_ENTRY_HPP

/**
 * @struct MeshEntry
 * @brief Represents metadata about a submesh section of a 3D model.
 *
 * Used to support models with multiple materials or mesh groups,
 * typically when importing from file formats like OBJ or FBX.
 */
struct MeshEntry
{
    unsigned int numIndices;     ///< Number of indices for this submesh.
    unsigned int baseVertex;     ///< Offset into the vertex buffer.
    unsigned int baseIndex;      ///< Offset into the index buffer.
    unsigned int materialIndex;  ///< Material index this submesh uses.
};

#endif // MESH_ENTRY_HPP
