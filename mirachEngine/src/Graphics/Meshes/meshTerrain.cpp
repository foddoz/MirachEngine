#include "meshTerrain.hpp"

#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <ctime>

//#include "../graphics.hpp"

#include "terrain_chunk.hpp"

// Constructor initializing terrain size and texture scale
MeshTerrain::MeshTerrain()
	: 
	Mesh3D(), 
	m_terrainSize(0), 
	m_textureScale(1.0f), 
	m_depth(0), 
	m_width(0), 
	m_minHeight(0), 
	m_maxHeight(0), 
	m_worldSizeX(1.0f), 
	m_worldSizeZ(1.0f) 
{

}

// Generates terrain data: heightmap, geometry, and AABB
void MeshTerrain::GenerateCPUData(int terrainSize, int iterations, float minHeight, float maxHeight, float filter, float textureScale, const std::vector<std::string>& textureFileNames, bool randomise)
{
	m_textureScale = textureScale;
	m_textureFileNames = textureFileNames;

	for (const auto& fileName : m_textureFileNames)
	{
		Texture* tex = new Texture(TextureTarget::Texture2D);
		tex->LoadFromFile(fileName);
		m_textures.push_back(tex);
	}

	// Apply fault formation algorithm to generate terrain heights
	CreateFaultFormation(iterations, minHeight, maxHeight, filter, terrainSize, randomise);

	// Generate mesh from heightmap (vertices + indices)
	CreateTriangleList(terrainSize, terrainSize);
}

// Uploads mesh data and textures to the GPU
void MeshTerrain::UploadToGPU()
{
	PopulateBuffers();

	Graphics::BufferData(BufferTarget::ArrayBuffer,
		sizeof(Vertex) * m_cpuVertexData.size(),
		m_cpuVertexData.data(),
		BufferUsage::StaticDraw);

	Graphics::BufferData(BufferTarget::ElementArrayBuffer,
		sizeof(unsigned int) * m_cpuIndexData.size(),
		m_cpuIndexData.data(),
		BufferUsage::StaticDraw);

	m_indexCount = static_cast<GLsizei>(m_cpuIndexData.size());

	for (auto& tex : m_textures) 
	{
		if (tex)
		{
			tex->UploadToGPU();
		}
	}

	Graphics::UnbindVertexArray();
	Graphics::UnbindBuffer(BufferTarget::ArrayBuffer);
	Graphics::UnbindBuffer(BufferTarget::ElementArrayBuffer);
}

void MeshTerrain::ClearCPUData()
{
	m_cpuVertexData.clear();
	m_cpuIndexData.clear();
}

// Initializes vertex and index buffers for triangle mesh
void MeshTerrain::CreateTriangleList(int width, int depth)
{
	m_width = width;
	m_depth = depth;

	assert(width > 0 && depth > 0);

	m_cpuVertexData.resize(m_width * m_depth);
	m_cpuIndexData.resize((m_width - 1) * (m_depth - 1) * 6);

	InitVertices(m_cpuVertexData);
	CalcNormals(m_cpuVertexData, m_width, m_depth);
	InitIndices(m_cpuIndexData, m_width, m_depth);
}

// Fault formation terrain generation (procedural)
void MeshTerrain::CreateFaultFormation(int iterations, float minHeight, float maxHeight, float filter, int terrainSize, bool randomise)
{
	if (randomise) 
	{
		std::srand(static_cast<unsigned>(std::time(nullptr)));  // Different each run
	}
	else 
	{
		std::srand(12345);  // Same every run for consistent terrain
	}

	m_terrainSize = terrainSize;
	m_worldSizeX = static_cast<float>(terrainSize);  // Set world size here
	m_worldSizeZ = static_cast<float>(terrainSize);

	m_minHeight = (int)minHeight;
	m_maxHeight = (int)maxHeight;

	m_width = terrainSize;
	m_depth = terrainSize;
	// Initialize heightmap with zeros
	m_heightMap.resize(m_width * m_depth, 0.0f);

	float deltaHeight = maxHeight - minHeight;

	// === Precompute all fault lines ===
	std::vector<std::pair<TerrainPoint, TerrainPoint>> faultLines;
	faultLines.reserve(iterations);

	for (int i = 0; i < iterations; ++i) 
	{
		TerrainPoint p1, p2;
		GenRandomTerrainPoints(p1, p2);
		faultLines.emplace_back(p1, p2);
	}

	for (int curIter = 0; curIter < iterations; curIter++)
	{
		// Interpolated fault height based on iteration progress
		float iterationRatio = ((float)curIter / (float)iterations);
		float height = maxHeight - iterationRatio * deltaHeight;

		const auto& [p1, p2] = faultLines[curIter];

		int dirX = p2.x - p1.x;
		int dirZ = p2.z - p1.z;

		for (int z = 0; z < m_terrainSize; z++)
		{
			for (int x = 0; x < m_terrainSize; x++)
			{
				int dirX_in = x - p1.x;
				int dirZ_in = z - p1.z;

				// Cross product determines which side of the fault the point lies on
				int crossProduct = dirX_in * dirZ - dirX * dirZ_in;

				if (crossProduct > 0)
				{
					m_heightMap[z * m_width + x] += height;
				}
			}
		}
	}

	ApplyFIRFilter(filter); // Smooth the terrain with FIR filtering

	// Normalize heightmap to min/max range
	NormaliseHeightMap(minHeight, maxHeight);
}

// Generates two unique random points for fault lines
void MeshTerrain::GenRandomTerrainPoints(TerrainPoint& p1, TerrainPoint& p2)
{
	p1.x = rand() % m_terrainSize;
	p1.z = rand() % m_terrainSize;

	int counter = 0;

	do
	{
		p2.x = rand() % m_terrainSize;
		p2.z = rand() % m_terrainSize;

		if (counter++ == 1000)
		{
			std::cout << "Endless loop detected\n";
			assert(0);
		}
	} while (p1.isEqual(p2));
}

// Applies a Finite Impulse Response filter to smooth terrain in all directions
void MeshTerrain::ApplyFIRFilter(float filter)
{
	for (int z = 0; z < m_terrainSize; z++)
	{
		float prevVal = m_heightMap[z * m_width + 0];
		for (int x = 1; x < m_terrainSize; x++)
		{
			prevVal = FIRFilterSinglePoint(x, z, prevVal, filter);
		}
	}

	for (int z = 0; z < m_terrainSize; z++)
	{
		float prevVal = m_heightMap[z * m_width + (m_terrainSize - 1)];
		for (int x = m_terrainSize - 2; x >= 0; x--)
		{
			prevVal = FIRFilterSinglePoint(x, z, prevVal, filter);
		}
	}

	for (int x = 0; x < m_terrainSize; x++)
	{
		float prevVal = m_heightMap[0 * m_width + x];
		for (int z = 1; z < m_terrainSize; z++)
		{
			prevVal = FIRFilterSinglePoint(x, z, prevVal, filter);
		}
	}

	for (int x = 0; x < m_terrainSize; x++)
	{
		float prevVal = m_heightMap[(m_terrainSize - 1) * m_width + x];
		for (int z = m_terrainSize - 2; z >= 0; z--)
		{
			prevVal = FIRFilterSinglePoint(x, z, prevVal, filter);
		}
	}
}

// Single FIR filtering step at (x, z)
float MeshTerrain::FIRFilterSinglePoint(int x, int z, float prevVal, float filter)
{
	float curVal = GetHeight(x, z);
	float newVal = filter * prevVal + (1 - filter) * curVal;
	SetHeight(x, z, newVal);
	return newVal;
}

// Creates VAO, VBO, and IBO and sets vertex attribute pointers
void MeshTerrain::PopulateBuffers()
{
	Graphics::GenVertexArray(m_vao);
	Graphics::BindVertexArray(m_vao);

	Graphics::GenBuffer(m_vbo);
	Graphics::BindBuffer(BufferTarget::ArrayBuffer, m_vbo);

	Graphics::GenBuffer(m_ibo);
	Graphics::BindBuffer(BufferTarget::ElementArrayBuffer, m_ibo);

	// === Vertex Attribute Layouts ===
	Graphics::EnableVertexAttrib(POSITION_LOCATION);
	Graphics::SetVertexAttribPointer(POSITION_LOCATION, 3, DataType::Float, false, sizeof(Vertex), (const void*)offsetof(Vertex, pos));

	Graphics::EnableVertexAttrib(TEX_COORD_LOCATION);
	Graphics::SetVertexAttribPointer(TEX_COORD_LOCATION, 2, DataType::Float, false, sizeof(Vertex), (const void*)offsetof(Vertex, tex));

	Graphics::EnableVertexAttrib(NORMAL_LOCATION);
	Graphics::SetVertexAttribPointer(NORMAL_LOCATION, 3, DataType::Float, false, sizeof(Vertex), (const void*)offsetof(Vertex, normal));

}

// Initializes a single vertex from heightmap data
void MeshTerrain::Vertex::InitVertex(int x, int z, const MeshTerrain* terrain)
{
	if (!terrain) 
	{
		std::cerr << "[InitVertex] ERROR: terrain is null!\n";
		pos = Vector3f(0.0f, 0.0f, 0.0f);
		tex = Vector2f(0.0f, 0.0f);
		normal = Vector3f(0.0f, 1.0f, 0.0f);
		return;
	}

	float y = terrain->GetHeight(x, z);

	float worldX = static_cast<float>(x);
	float worldZ = static_cast<float>(z);

	if (const TerrainChunk* chunk = dynamic_cast<const TerrainChunk*>(terrain)) 
	{
		worldX += static_cast<float>(chunk->GetOrigin().x);
		worldZ += static_cast<float>(chunk->GetOrigin().z);
	}

	pos = Vector3f(worldX, y, worldZ);

	float size = static_cast<float>(terrain->GetSize());
	float textureScale = terrain->GetTextureScale();
	tex = Vector2f(textureScale * static_cast<float>(x) / size, textureScale * static_cast<float>(z) / size);
}



// Initializes vertices and expands the terrain AABB
void MeshTerrain::InitVertices(std::vector<Vertex>& Vertices)
{
	m_AABB = AABB(); // Reset bounding box

	int index = 0;
	for (int z = 0; z < m_depth; z++)
	{
		for (int x = 0; x < m_width; x++)
		{
			Vertices[index].InitVertex(x, z, this);
			m_AABB.Expand(Vector3f(Vertices[index].pos.x, Vertices[index].pos.y, Vertices[index].pos.z));
			index++;
		}
	}
}

// Generates index buffer for triangle grid
void MeshTerrain::InitIndices(std::vector<unsigned int>& indices, int width, int depth)
{
	int index = 0;
	for (int z = 0; z < depth - 1; z++)
	{
		for (int x = 0; x < width - 1; x++)
		{
			unsigned int bl = z * width + x;
			unsigned int tl = (z + 1) * width + x;
			unsigned int tr = (z + 1) * width + x + 1;
			unsigned int br = z * width + x + 1;

			indices.push_back(bl);
			indices.push_back(tl);
			indices.push_back(tr);

			indices.push_back(bl);
			indices.push_back(tr);
			indices.push_back(br);
		}
	}
}

// Computes vertex normals by averaging face normals
void MeshTerrain::CalcNormals(std::vector<Vertex>& Vertices, int width, int depth)
{
	for (auto& v : Vertices)
	{
		v.normal = Vector3f{ 0.0f, 0.0f, 0.0f };
	}

	for (int z = 0; z < depth - 1; z++)
	{
		for (int x = 0; x < width - 1; x++)
		{
			int idx0 = z * width + x;
			int idx1 = (z + 1) * width + x;
			int idx2 = (z + 1) * width + (x + 1);
			int idx3 = z * width + (x + 1);

			Vector3f p0 = Vertices[idx0].pos;
			Vector3f p1 = Vertices[idx1].pos;
			Vector3f p2 = Vertices[idx2].pos;
			Vector3f p3 = Vertices[idx3].pos;

			Vector3f n1 = Normalize(Cross(p1 - p0, p2 - p0));
			Vector3f n2 = Normalize(Cross(p2 - p0, p3 - p0));

			Vertices[idx0].normal = Vertices[idx0].normal + n1 + n2;
			Vertices[idx1].normal = Vertices[idx1].normal + n1;
			Vertices[idx2].normal = Vertices[idx2].normal + n1 + n2;
			Vertices[idx3].normal = Vertices[idx3].normal + n2;
		}
	}

	for (auto& v : Vertices)
	{
		v.normal = Normalize(v.normal);
	}
}

// Accessors and utilities

float MeshTerrain::GetHeight(int x, int z) const
{
	// Warning if original inputs are out of bounds
	if (x < 0 || x >= m_width || z < 0 || z >= m_depth) 
	{
		std::cerr << "[MeshTerrain::GetHeight] WARNING: out-of-bounds access! "
			<< "Requested (" << x << ", " << z << "), "
			<< "Clamping to [0, " << (m_width - 1) << "] and [0, " << (m_depth - 1) << "]\n";
	}

	int safeX = (int)Clamp((float)x, 0.f, (float)(m_width - 1));
	int safeZ = (int)Clamp((float)z, 0.f, (float)(m_depth - 1));

	return m_heightMap[safeZ * m_width + safeX];
}

float MeshTerrain::GetWorldHeight(float x, float z) const 
{
	float stepX = m_worldSizeX / static_cast<float>(m_width);
	float stepZ = m_worldSizeZ / static_cast<float>(m_depth);

	float gridX = x / stepX;
	float gridZ = z / stepZ;

	return GetHeightInterpolated(gridX, gridZ);
}

// Bilinear height interpolation
float MeshTerrain::GetHeightInterpolated(float x, float z) const
{
	float baseHeight = GetHeight((int)x, (int)z);

	if (((int)x + 1 >= m_terrainSize) || ((int)z + 1 >= m_terrainSize)) 
	{
		return baseHeight;
	}

	float nextXHeight = GetHeight((int)x + 1, (int)z);
	float ratioX = x - floorf(x);
	float InterpolatedHeightX = (float)(nextXHeight - baseHeight) * ratioX + baseHeight;

	float NextZHeight = GetHeight((int)x, (int)z + 1);
	float RatioZ = z - floorf(z);
	float InterpolatedHeightZ = (float)(NextZHeight - baseHeight) * RatioZ + baseHeight;

	return (InterpolatedHeightX + InterpolatedHeightZ) / 2.0f;
}

int MeshTerrain::GetSize() const 
{ 
	return m_terrainSize; 
}

int MeshTerrain::GetWidth() const 
{ 
	return m_width; 
}
int MeshTerrain::GetDepth() const 
{ 
	return m_depth; 
}

float MeshTerrain::GetTextureScale() const 
{ 
	return m_textureScale; 
}

void MeshTerrain::SetHeight(int x, int z, float value)
{
	m_heightMap[z * m_width + x] = value;
}

void MeshTerrain::NormaliseHeightMap(float minHeight, float maxHeight)
{
	float currentMin = std::numeric_limits<float>::max();
	float currentMax = std::numeric_limits<float>::lowest();

	// Step 1: Find current min and max
	for (float h : m_heightMap) 
	{
		if (h < currentMin)
		{
			currentMin = h;
		}
		if (h > currentMax)
		{
			currentMax = h;
		}
	}

	float range = currentMax - currentMin;
	float targetRange = maxHeight - minHeight;

	// Step 2: Normalize values
	for (float& h : m_heightMap) 
	{
		h = ((h - currentMin) / range) * targetRange + minHeight;
	}
}

float MeshTerrain::GetWorldWidth() const
{
	return m_worldSizeX;
}

float MeshTerrain::GetWorldDepth() const
{
	return m_worldSizeZ;
}

int MeshTerrain::GetMinHeight() const
{
	return m_minHeight;
}

int MeshTerrain::GetMaxHeight() const
{
	return m_maxHeight;
}

const std::vector<float>& MeshTerrain::GetHeightMap() const 
{ 
	return m_heightMap; 
}

void MeshTerrain::Delete() 
{ 
	Mesh3D::Delete(); 
}

// Converts terrain into a MeshRenderer struct for ECS use
MeshRenderer MeshTerrain::ToRenderer()
{
	MeshRenderer renderer;

	// === Fill one MeshLOD for now ===
	MeshLOD lod;
	lod.vao = m_vao;
	lod.vbo = m_vbo;
	lod.ibo = m_ibo;
	lod.indexCount = m_indexCount;
	lod.localAABB = m_AABB;

	//renderer.LODs.push_back(lod); // Add the main LOD (LOD0)
	renderer.lods.push_back(CreateLOD(1));
	renderer.lods.push_back(CreateLOD(2)); //Half resolution
	renderer.lods.push_back(CreateLOD(6)); 

	// === Shared data ===
	renderer.pipeline = m_pipeline;
	renderer.textures = m_textures;
	renderer.minHeight = static_cast<float>(m_minHeight);
	renderer.maxHeight = static_cast<float>(m_maxHeight);

	for (int i = 0; i < m_textures.size(); ++i)
	{
		renderer.textureUniformNames.push_back("gTextureHeight" + std::to_string(i));
	}

	return renderer;
}

MeshLOD MeshTerrain::CreateLOD(int stepSize)
{
	MeshLOD lod;

	std::vector<Vertex> lodVertices;
	std::vector<unsigned int> lodIndices;

	int lodWidth = (m_width + stepSize - 1) / stepSize;
	int lodDepth = (m_depth + stepSize - 1) / stepSize;

	lodVertices.reserve(lodWidth * lodDepth);

	// === Generate vertices ===
	for (int z = 0; z < m_depth; z += stepSize)
	{
		for (int x = 0; x < m_width; x += stepSize)
		{
			Vertex v;
			v.InitVertex(x, z, this);
			lodVertices.push_back(v);
		}
	}
	
	// === Generate indices ===
	InitIndices(lodIndices, lodWidth, lodDepth);

	CalcNormals(lodVertices, lodWidth, lodDepth);

	// === Upload to GPU ===
	using namespace Graphics;

	GenVertexArray(lod.vao);
	BindVertexArray(lod.vao);

	GenBuffer(lod.vbo);
	BindBuffer(BufferTarget::ArrayBuffer, lod.vbo);
	BufferData(BufferTarget::ArrayBuffer,
		lodVertices.size() * sizeof(Vertex),
		lodVertices.data(),
		BufferUsage::StaticDraw);

	GenBuffer(lod.ibo);
	BindBuffer(BufferTarget::ElementArrayBuffer, lod.ibo);
	BufferData(BufferTarget::ElementArrayBuffer,
		lodIndices.size() * sizeof(unsigned int),
		lodIndices.data(),
		BufferUsage::StaticDraw);

	// === Vertex Attributes ===
	EnableVertexAttrib(POSITION_LOCATION);
	SetVertexAttribPointer(POSITION_LOCATION, 3, DataType::Float, false, sizeof(Vertex), (const void*)offsetof(Vertex, pos));

	EnableVertexAttrib(TEX_COORD_LOCATION);
	SetVertexAttribPointer(TEX_COORD_LOCATION, 2, DataType::Float, false, sizeof(Vertex), (const void*)offsetof(Vertex, tex));

	EnableVertexAttrib(NORMAL_LOCATION);
	SetVertexAttribPointer(NORMAL_LOCATION, 3, DataType::Float, false, sizeof(Vertex), (const void*)offsetof(Vertex, normal));

	// === Mesh Metadata ===
	lod.indexCount = static_cast<unsigned int>(lodIndices.size());
	lod.localAABB = m_AABB;

	// === Unbind to finalize VAO setup ===
	UnbindVertexArray();

	return lod;
}



