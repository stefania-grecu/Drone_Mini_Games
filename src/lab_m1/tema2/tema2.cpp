#include "lab_m1/tema2/tema2.h"

#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace m1;


Tema2::Tema2()
{
}


Tema2::~Tema2()
{
}


void Tema2::Init()
{
    camera = new implemented::Camera_mea();
    minimapCamera = new implemented::Camera_mea();

    // create terrain
    n = 50;
	m = 50;
	width = 1;
	height = 1;
	CreateTerrain();
	CreateMesh("terrain", vertices, indices);

    // shader pentru cub
    {
        Shader* shader = new Shader("TerrainShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "terrain", "TerrainVertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "terrain", "TerrainFragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    // valori de min si maxim pentru teren
    minX = vertices.front().position.x;
    maxX = vertices.back().position.x;
    minZ = vertices.front().position.z;
    maxZ = vertices.back().position.z;

    // corpuri pentru brad
    // creare cilindru
    std::vector<VertexFormat> cylinderVertices;
    std::vector<unsigned int> cylinderIndices;
    // generare vertexi
    GenerateCylinder(cylinderVertices, cylinderIndices, 0.5f, 1.0f, 20);
    CreateMesh("cylinder", cylinderVertices, cylinderIndices);

    // creare con
    std::vector<VertexFormat> coneVertices;
    std::vector<unsigned int> coneIndices;
    //generare vertexi
    GenerateCone(coneVertices, coneIndices, 0.5f, 1.0f, 20);
    CreateMesh("cone", coneVertices, coneIndices);

	// corpuri pentru cladire
	// create cub
	std::vector<VertexFormat> cubeVertices;
	std::vector<unsigned int> cubeIndices;
	//generare vertexi
	GenerateCube(cubeVertices, cubeIndices, 1.0f);
	CreateMesh("cube", cubeVertices, cubeIndices);

    // creare piramida
    std::vector<VertexFormat> pyramidVertices;
    std::vector<unsigned int> pyramidIndices;
    // generare vertexi
    float baseLength = 2.0f;
    float height = 3.0f;
    GeneratePyramid(pyramidVertices, pyramidIndices, baseLength, height);
    CreateMesh("pyramid", pyramidVertices, pyramidIndices);

    int nr_tree = 40;
    int nr_buildings = 20;
    float minDistance = 5.0f;

    // genereaza pozitiile aleatoare ale copacilor
    for (int i = 0; i < nr_tree; ++i) {
        float x = (rand() % 490) / 10.0f;
        float z = (rand() % 490) / 10.0f;
        treePositions.push_back(glm::vec3(x, 0, z));
    }

	// genereaza pozitiile aleatoare ale cladirilor
    for (int i = 0; i < nr_buildings; ++i) {
        bool validPosition = false;
        glm::vec3 position;

        while (!validPosition) {
            float x = (rand() % 490) / 10.0f;
            float z = (rand() % 490) / 10.0f;
            position = glm::vec3(x, 0, z);

            validPosition = true;

			// distanta min fata de fiecare copac
            for (const glm::vec3& treePosition : treePositions) {
                float distance = glm::distance(position, treePosition);
                if (distance < minDistance) {
                    validPosition = false;
                    break;
                }
            }

            // distanta min fata de drona
            if (validPosition) {
                float distance = glm::distance(position, dronePosition);
                if (distance < minDistance) {
                    validPosition = false;
                }
            }

            // distanta min fata de alte cladiri
            if (validPosition) {
                for (const glm::vec3& p : buildingPositions) {
                    float distance = glm::distance(position, p);
                    if (distance < minDistance) {
                        validPosition = false;
                        break;
                    }
                }
            }
        }

        buildingPositions.push_back(position);
    }

	// generare pozitii checkpoints aleatorii dar care sa nu fie in apropierea copacilor sau a cladirilor
	for (int i = 0; i < nr_checkpoints; ++i) {
		bool validPosition = false;
		glm::vec3 position;

		while (!validPosition) {
			float x = (rand() % 490) / 10.0f;
			float z = (rand() % 490) / 10.0f;
			position = glm::vec3(x, 0, z);

			validPosition = true;

			// distanta min fata de fiecare copac
			for (const glm::vec3& treePosition : treePositions) {
				float distance = glm::distance(position, treePosition);
				if (distance < minDistance) {
					validPosition = false;
					break;
				}
			}

			// distanta min fata de drona
			if (validPosition) {
				float distance = glm::distance(position, dronePosition);
				if (distance < minDistance) {
					validPosition = false;
				}
			}

			// distanta min fata de alte cladiri
			if (validPosition) {
				for (const glm::vec3& p : buildingPositions) {
					float distance = glm::distance(position, p);
					if (distance < minDistance) {
						validPosition = false;
						break;
					}
				}
			}

			// distanta min fata de alte checkpoints
			if (validPosition) {
				for (const glm::vec3& p : checkpointPositions) {
					float distance = glm::distance(position, p);
					if (distance < minDistance) {
						validPosition = false;
						break;
					}
				}
			}
		}
        checkpointPositions.push_back(position);
	}

    // shader pentru cub
    {
        Shader* shader = new Shader("CubeShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "cube", "CubeVertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "cube", "CubeFragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    // drona
    dronePosition = glm::vec3(minX + (maxX - minX) / 2, 2.0f, minZ + (maxZ - minZ) / 2);
    droneBodyScale = glm::vec3(1, 0.1f, 0.1f);
    droneConnectorScale = glm::vec3(0.1f, 0.1f, 0.1f);
    dronePropellerScale = glm::vec3(0.1f, 0.02f, 0.6f);
    droneYaw = 0.0f;
    droneSpeed = 2.5f;

    // miniviewport
    polygonMode = GL_FILL;
    glm::ivec2 resolution = window->GetResolution();
    miniViewportArea = ViewportArea(50, 50, resolution.x / 5.f, resolution.y / 5.f);

	// sageata galbena
    Mesh* arrow = CreateArrow("arrow", glm::vec3(1, 1, 0));
    meshes["arrow"] = arrow;
}

Mesh* Tema2::CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned int>& indices)
{
    unsigned int VAO = 0;
    // Create the VAO and bind it
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Create the VBO and bind it
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Send vertices data into the VBO buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    // Create the IBO and bind it
    unsigned int IBO;
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    // Send indices data into the IBO buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);


    // Set vertex position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), 0);

    // Set vertex normal attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(glm::vec3)));

    // Set texture coordinate attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3)));

    // Set vertex color attribute
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3) + sizeof(glm::vec2)));

    // Unbind the VAO
    glBindVertexArray(0);

    // Check for OpenGL errors
    CheckOpenGLError();

    // Mesh information is saved into a Mesh object
    meshes[name] = new Mesh(name);
    meshes[name]->InitFromBuffer(VAO, static_cast<unsigned int>(indices.size()));
    meshes[name]->vertices = vertices;
    meshes[name]->indices = indices;
    return meshes[name];
}

Mesh* Tema2::CreateArrow(const std::string& name, const glm::vec3& color) {
	// vertexii pentru sageata
    std::vector<VertexFormat> vertices = {
        VertexFormat(glm::vec3(0, 1, 0), color),      // varf
		VertexFormat(glm::vec3(-0.5f, 0, 0), color), // colt stanga
        VertexFormat(glm::vec3(0, 0.3f, 0), color), // baza
		VertexFormat(glm::vec3(0.5f, 0, 0), color),  // colt dreapta
    };

	// indecsii pentru sageata
    std::vector<unsigned int> indices = {
        0, 1, 2,
		0, 2, 3
    };

    Mesh* arrow = new Mesh(name);
    arrow->InitFromData(vertices, indices);
    return arrow;
}

void Tema2::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0.6f, 0.8f, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}

void Tema2::RenderScene(float deltaTimeSeconds) {

    // terrain
    glm::mat4 terrainModelMatrix = glm::mat4(1);
	terrainModelMatrix = glm::translate(terrainModelMatrix, glm::vec3(0, 0, 0));
    RenderSimpleMesh(meshes["terrain"], shaders["TerrainShader"], terrainModelMatrix, glm::vec3(0, 0, 0));

    // trees
    for (const auto& position : treePositions) {
        CreateTree(position, glm::vec3(0.1f, 0.6f, 0.1f), glm::vec3(0.5f, 0.5f, 0.5f));
    }
    // buildings
    for (const auto& position : buildingPositions) {
        CreateBuilding(position, glm::vec3(0.7f, 3.0f, 0.7f), glm::vec3(0.5f, 0.3f, 0.5f));
    }
	// checkpoints
	for (int i = 0; i < checkpointPositions.size(); i++) {
        if (i == score)
			CreateCheckPoint(checkpointPositions[i], glm::vec3(0, 1, 0));       // verde
		else
			CreateCheckPoint(checkpointPositions[i], glm::vec3(1, 0, 0));	   // rosu
	}

    // drone
    CreateDrone(dronePosition, droneBodyScale, droneConnectorScale, dronePropellerScale, deltaTimeSeconds);

	// sageata
	RenderDirectionArrow(dronePosition, score);
}

void Tema2::Update(float deltaTimeSeconds)
{
    glLineWidth(10);
    glPointSize(5);
    glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

	// primul viewport - camera first person
    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);

    GetSceneCamera()->SetPosition(camera->GetPosition());
    GetSceneCamera()->SetRotation(glm::quatLookAt(glm::normalize(camera->forward), glm::vec3(0, 1, 0)));
    RenderScene(deltaTimeSeconds);

	// Miniviewport - camera third person
    glClear(GL_DEPTH_BUFFER_BIT);
    glViewport(miniViewportArea.x, miniViewportArea.y, miniViewportArea.width, miniViewportArea.height);

	// seteaza camera third person
	glm::vec3 forward = glm::normalize(camera->forward); // directia in care se uita camera
	glm::vec3 thirdPersonCameraPosition = dronePosition - forward * 5.0f; // pozitia camerei
	glm::vec3 thirdPersonCameraTarget = dronePosition; // punctul catre care se uita camera
	glm::vec3 thirdPersonUp = glm::vec3(0, 1, 0); // vectorul up al camerei

    minimapCamera->Set(thirdPersonCameraPosition, thirdPersonCameraTarget, thirdPersonUp);

    glm::mat4 minimapViewMatrix = minimapCamera->GetViewMatrix();
    glm::mat4 minimapProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, 0.1f, 100.0f);

	// setez camera third person
    GetSceneCamera()->SetPosition(thirdPersonCameraPosition);
    GetSceneCamera()->SetRotation(glm::quatLookAt(glm::normalize(thirdPersonCameraTarget - thirdPersonCameraPosition), thirdPersonUp));

    RenderScene(deltaTimeSeconds);
}

void Tema2::FrameEnd()
{
    DrawCoordinateSystem(camera->GetViewMatrix(), projectionMatrix);
}

void Tema2::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    // TODO(student): Get shader location for uniform mat4 "Model"
    int location = glGetUniformLocation(shader->program, "Model");

    // TODO(student): Set shader uniform "Model" to modelMatrix
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // TODO(student): Get shader location for uniform mat4 "View"
    int viewLocation = glGetUniformLocation(shader->program, "View");

    // TODO(student): Set shader uniform "View" to viewMatrix
    glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // TODO(student): Get shader location for uniform mat4 "Projection"
    int projectionLocation = glGetUniformLocation(shader->program, "Projection");

    // TODO(student): Set shader uniform "Projection" to projectionMatrix
    glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // TODO(student): Get shader location for uniform vec3 "Color"
    int colorLocation = glGetUniformLocation(shader->program, "objectColor");
    glUniform3fv(colorLocation, 1, glm::value_ptr(color));

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

void Tema2::CreateTerrain() {
	//  vertices
    for (int i = 0; i <= m; ++i) {
        for (int j = 0; j <= n; ++j) {
            float x = j * width;
            float z = i * height;
            vertices.emplace_back(glm::vec3(x, 0, z), glm::vec3(0, 1, 0), glm::vec3(0, 0.7f, 0));
        }
    }

	// indices
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            unsigned int idx1 = i * (n + 1) + j;
            unsigned int idx2 = idx1 + 1;
            unsigned int idx3 = idx1 + (n + 1);
            unsigned int idx4 = idx3 + 1;

            if (i == 0 && j == 0) {
                cout << idx1 << " " << idx2 << " " << idx3 << " " << idx4 << endl;
            }

            indices.push_back(idx1);
            indices.push_back(idx2);
            indices.push_back(idx3);

            indices.push_back(idx2);
            indices.push_back(idx4);
            indices.push_back(idx3);
        }
    }
}

void Tema2::GenerateCylinder(std::vector<VertexFormat>& vertices, std::vector<unsigned int>& indices, float radius, float height, int segments) {
	// bazele cilindrului
    vertices.emplace_back(glm::vec3(0, 0, 0), glm::vec3(0, -1, 0), glm::vec3(1.0f, 0.5f, 0.2f));        // normala in jos
	vertices.emplace_back(glm::vec3(0, height, 0), glm::vec3(0, 1, 0), glm::vec3(1.0f, 0.5f, 0.2f));	// normala in sus

    for (int i = 0; i <= segments; ++i) {
        float angle = i * 2.0f * glm::pi<float>() / segments;
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        
        vertices.emplace_back(glm::vec3(x, 0, z), glm::vec3(0, -1, 0), glm::vec3(0.5f, 0.3f, 0.1f));
        vertices.emplace_back(glm::vec3(x, height, z), glm::vec3(0, 1, 0), glm::vec3(0.5f, 0.3f, 0.1f));

        if (i < segments) {
            int idx = i * 2 + 2; // offset +2 pentru centrele bazelor

            // laterale
            indices.push_back(idx);
            indices.push_back(idx + 1);
            indices.push_back(idx + 3);

            indices.push_back(idx);
            indices.push_back(idx + 3);
            indices.push_back(idx + 2);

            // baza de jos
			indices.push_back(0); // centrul bazei de jos
            indices.push_back(idx);
			indices.push_back((idx + 2) % (segments * 2 + 2)); // urmatorele punct de pe cerc

            // baza de sus
			indices.push_back(1); // centrul bazei de sus
            indices.push_back(idx + 1);
			indices.push_back((idx + 3) % (segments * 2 + 2)); // urmatorele punct de pe cerc
        }
    }
}

void Tema2::GenerateCone(std::vector<VertexFormat>& vertices, std::vector<unsigned int>& indices, float radius, float height, int segments) {
	// varful conului
    vertices.emplace_back(glm::vec3(0, height, 0), glm::vec3(0, 1, 0));

    // baza conului
    for (int i = 0; i <= segments; ++i) {
        float angle = i * 2.0f * glm::pi<float>() / segments;
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        vertices.emplace_back(glm::vec3(x, 0, z), glm::vec3(0, -1, 0));

        // crearea triunghiurilor
        if (i < segments) {
			indices.push_back(0);             // varful conului
			indices.push_back(i + 1);         // punctul curent din baza
            indices.push_back(i + 2);         // urmatorul punct din baza
        }
    }

	// triunghiurile laterale
    for (int i = 1; i < segments; ++i) {
        indices.push_back(i);           // punctul curent din baza
        indices.push_back(i + 1);       // punctul urmator din baza
        indices.push_back(segments + 1); // centru bazei
    }
}

void Tema2::GenerateCube(std::vector<VertexFormat>& vertices, std::vector<unsigned int>& indices, float side) {
    float halfSide = side / 2.0f;

    vertices = {
        VertexFormat(glm::vec3(-halfSide, -halfSide,  halfSide), glm::vec3(1, 0, 0)), // 0
        VertexFormat(glm::vec3(halfSide, -halfSide,  halfSide), glm::vec3(0, 1, 0)), // 1
        VertexFormat(glm::vec3(halfSide,  halfSide,  halfSide), glm::vec3(0, 0, 1)), // 2
        VertexFormat(glm::vec3(-halfSide,  halfSide,  halfSide), glm::vec3(1, 1, 0)), // 3

        VertexFormat(glm::vec3(-halfSide, -halfSide, -halfSide), glm::vec3(1, 0, 1)), // 4
        VertexFormat(glm::vec3(halfSide, -halfSide, -halfSide), glm::vec3(0, 1, 1)), // 5
        VertexFormat(glm::vec3(halfSide,  halfSide, -halfSide), glm::vec3(1, 1, 1)), // 6
        VertexFormat(glm::vec3(-halfSide,  halfSide, -halfSide), glm::vec3(0, 0, 0)), // 7
    };

    indices = {
        0, 1, 2,
        2, 3, 0,
        4, 5, 6,
        6, 7, 4,
        0, 3, 7,
        7, 4, 0,
        1, 5, 6,
        6, 2, 1,
        0, 4, 5,
        5, 1, 0,
        3, 2, 6,
        6, 7, 3
    };
}

void Tema2::GeneratePyramid(std::vector<VertexFormat>& vertices, std::vector<unsigned int>& indices, float baseLength, float height) {
	// varful piramidei
    vertices.emplace_back(glm::vec3(0, height, 0), glm::vec3(0, 1, 0));

	// cele 4 puncte ale bazei piramidei
    float halfBase = baseLength / 2.0f;
    vertices.emplace_back(glm::vec3(-halfBase, 0, -halfBase), glm::vec3(0, -1, 0));
    vertices.emplace_back(glm::vec3(halfBase, 0, -halfBase), glm::vec3(0, -1, 0));
    vertices.emplace_back(glm::vec3(halfBase, 0, halfBase), glm::vec3(0, -1, 0));
    vertices.emplace_back(glm::vec3(-halfBase, 0, halfBase), glm::vec3(0, -1, 0));

	// triunghiurile pentru fiecare fatada
    indices.push_back(0); indices.push_back(1); indices.push_back(2);
    indices.push_back(0); indices.push_back(2); indices.push_back(3);
    indices.push_back(0); indices.push_back(3); indices.push_back(4);
    indices.push_back(0); indices.push_back(4); indices.push_back(1);

    // triunghiurile pentru baza piramidei
    indices.push_back(1); indices.push_back(2); indices.push_back(3);
    indices.push_back(1); indices.push_back(3); indices.push_back(4);
}

void Tema2::CreateTree(const glm::vec3& position, const glm::vec3& scale_trunk, const glm::vec3& scale_foliage) {
    glm::mat4 trunkModelMatrix = glm::mat4(1);
    trunkModelMatrix = glm::translate(trunkModelMatrix, position);
    trunkModelMatrix = glm::scale(trunkModelMatrix, scale_trunk);
    RenderSimpleMesh(meshes["cylinder"], shaders["CubeShader"], trunkModelMatrix, glm::vec3(0.4f, 0.2f, 0));

    glm::mat4 bottomConeModelMatrix = glm::mat4(1);
    bottomConeModelMatrix = glm::translate(bottomConeModelMatrix, position + glm::vec3(0, scale_trunk.y, 0));
    bottomConeModelMatrix = glm::scale(bottomConeModelMatrix, scale_foliage);
    RenderSimpleMesh(meshes["cone"], shaders["CubeShader"], bottomConeModelMatrix, glm::vec3(0.0f, 0.4f, 0));

    glm::mat4 topConeModelMatrix = glm::mat4(1);
    topConeModelMatrix = glm::translate(topConeModelMatrix, position + glm::vec3(0, scale_trunk.y + scale_foliage.y - 0.2f, 0));
    topConeModelMatrix = glm::scale(topConeModelMatrix, scale_foliage * 0.7f);
    RenderSimpleMesh(meshes["cone"], shaders["CubeShader"], topConeModelMatrix, glm::vec3(0.0f, 0.43f, 0));

    glm::mat4 additionalConeModelMatrix = glm::mat4(1);
    additionalConeModelMatrix = glm::translate(additionalConeModelMatrix, position + glm::vec3(0, scale_trunk.y + scale_foliage.y, 0));
    additionalConeModelMatrix = glm::scale(additionalConeModelMatrix, scale_foliage * 0.5f);
    RenderSimpleMesh(meshes["cone"], shaders["CubeShader"], additionalConeModelMatrix, glm::vec3(0.0f, 0.45f, 0));
}

void Tema2::CreateBuilding(const glm::vec3& position, const glm::vec3& scale_base, const glm::vec3& scale_roof) {
    glm::mat4 baseModelMatrix = glm::mat4(1);
    baseModelMatrix = glm::translate(baseModelMatrix, position);
    baseModelMatrix = glm::scale(baseModelMatrix, scale_base);
    RenderSimpleMesh(meshes["cube"], shaders["CubeShader"], baseModelMatrix, glm::vec3(0.4f, 0, 0.2f));

    glm::mat4 roofModelMatrix = glm::mat4(1);
    roofModelMatrix = glm::translate(roofModelMatrix, position + glm::vec3(0, scale_base.y - 1.5, 0));
    roofModelMatrix = glm::scale(roofModelMatrix, scale_roof);
    RenderSimpleMesh(meshes["pyramid"], shaders["CubeShader"], roofModelMatrix, glm::vec3(0.2f, 0.2f, 0.2f));
}

void Tema2::CreateDrone(const glm::vec3& position, const glm::vec3& scale_body, const glm::vec3& scale_connector, const glm::vec3& scale_propeller, float deltaTime) {
	// corpul drona
    glm::mat4 bodyModelMatrix = glm::mat4(1);
    bodyModelMatrix = glm::translate(bodyModelMatrix, position);
    bodyModelMatrix = glm::rotate(bodyModelMatrix, glm::radians(droneYaw), glm::vec3(0, 1, 0));

    glm::mat4 bodyPart1Matrix = bodyModelMatrix;
    bodyPart1Matrix = glm::rotate(bodyPart1Matrix, glm::radians(45.0f), glm::vec3(0, 1, 0));
    bodyPart1Matrix = glm::scale(bodyPart1Matrix, scale_body);
    RenderSimpleMesh(meshes["cube"], shaders["CubeShader"], bodyPart1Matrix, glm::vec3(0.5f, 0.5f, 0.5f));

    glm::mat4 bodyPart2Matrix = bodyModelMatrix;
    bodyPart2Matrix = glm::rotate(bodyPart2Matrix, glm::radians(-45.0f), glm::vec3(0, 1, 0));
    bodyPart2Matrix = glm::scale(bodyPart2Matrix, scale_body);
    RenderSimpleMesh(meshes["cube"], shaders["CubeShader"], bodyPart2Matrix, glm::vec3(0.5f, 0.5f, 0.5f));

    glm::vec3 connectorOffsets[4] = {
        glm::vec3(-scale_body.x + 0.555f, scale_body.y, 0),
        glm::vec3(scale_body.x - 0.555f, scale_body.y, 0),
        glm::vec3(-scale_body.x + 0.555f, scale_body.y, -scale_body.z + 0.1f),
        glm::vec3(scale_body.x - 0.555f, scale_body.y, scale_body.z - 0.1f)
    };

    float rotations[4] = {
        glm::radians(45.0f),
        glm::radians(-45.0f),
        glm::radians(-135.0f),
        glm::radians(135.0f)
    };

    for (int i = 0; i < 4; ++i) {
        glm::mat4 connectorModelMatrix = bodyModelMatrix;
        connectorModelMatrix = glm::rotate(connectorModelMatrix, rotations[i], glm::vec3(0, 1, 0));
        connectorModelMatrix = glm::translate(connectorModelMatrix, connectorOffsets[i]);
        connectorModelMatrix = glm::scale(connectorModelMatrix, scale_connector);
        RenderSimpleMesh(meshes["cube"], shaders["CubeShader"], connectorModelMatrix, glm::vec3(0.5f, 0.5f, 0.5f));

		connectorOffsets[i].y += 0.05f;
    }

    static float propellerAngle = 0.0f;
    float rotationSpeed = 500.0f;
    propellerAngle += rotationSpeed * deltaTime;
    if (propellerAngle >= 360.0f) propellerAngle -= 360.0f;

	// elicea
    for (int i = 0; i < 4; ++i) {
        glm::mat4 propellerModelMatrix = bodyModelMatrix;
        propellerModelMatrix = glm::rotate(propellerModelMatrix, rotations[i], glm::vec3(0, 1, 0));
        propellerModelMatrix = glm::translate(propellerModelMatrix, connectorOffsets[i]);
        propellerModelMatrix = glm::rotate(propellerModelMatrix, glm::radians(propellerAngle), glm::vec3(0, 1, 0));
        propellerModelMatrix = glm::scale(propellerModelMatrix, scale_propeller);
        RenderSimpleMesh(meshes["cube"], shaders["CubeShader"], propellerModelMatrix, glm::vec3(0, 0, 0));
    }
}

void Tema2::CreateCheckPoint(const glm::vec3& position, glm::vec3 color) {
    glm::vec3 scale1 = glm::vec3(0.04f, 4.0f, 0.04f);
    glm::vec3 scale2 = glm::vec3(2.0f, 0.04f, 0.04f);
    glm::vec3 scale3 = glm::vec3(1.8f, 0.04f, 0.04f);
    glm::vec3 scale4 = glm::vec3(0.04f, 1.8f, 0.04f);

    glm::vec3 position1 = glm::vec3(0, 2, 0);
    glm::vec3 position2 = glm::vec3(2, 0, 0);

    // 1
    glm::mat4 checkpointModelMatrix = glm::mat4(1);
    checkpointModelMatrix = glm::translate(checkpointModelMatrix, position);
    checkpointModelMatrix = glm::scale(checkpointModelMatrix, scale1);
    RenderSimpleMesh(meshes["cube"], shaders["CubeShader"], checkpointModelMatrix, glm::vec3(0.5f, 0.5f, 0.5f));

    // 2
    checkpointModelMatrix = glm::mat4(1);
    checkpointModelMatrix = glm::translate(checkpointModelMatrix, position + position2);
    checkpointModelMatrix = glm::scale(checkpointModelMatrix, scale1);
    RenderSimpleMesh(meshes["cube"], shaders["CubeShader"], checkpointModelMatrix, glm::vec3(0.5f, 0.5f, 0.5f));

    // 3
    checkpointModelMatrix = glm::mat4(1);
    checkpointModelMatrix = glm::translate(checkpointModelMatrix, position + position1);
    checkpointModelMatrix = glm::scale(checkpointModelMatrix, scale1);
    RenderSimpleMesh(meshes["cube"], shaders["CubeShader"], checkpointModelMatrix, glm::vec3(0.5f, 0.5f, 0.5f));

    // 4
    checkpointModelMatrix = glm::mat4(1);
    checkpointModelMatrix = glm::translate(checkpointModelMatrix, position + position2 + position1);
    checkpointModelMatrix = glm::scale(checkpointModelMatrix, scale1);
    RenderSimpleMesh(meshes["cube"], shaders["CubeShader"], checkpointModelMatrix, glm::vec3(0.5f, 0.5f, 0.5f));

    // 5
    checkpointModelMatrix = glm::mat4(1);
    checkpointModelMatrix = glm::translate(checkpointModelMatrix, position + position1 + (position2 * 0.5f));
    checkpointModelMatrix = glm::scale(checkpointModelMatrix, scale2);
    RenderSimpleMesh(meshes["cube"], shaders["CubeShader"], checkpointModelMatrix, glm::vec3(0.5f, 0.5f, 0.5f));

    // 6
    checkpointModelMatrix = glm::mat4(1);
    checkpointModelMatrix = glm::translate(checkpointModelMatrix, position + position1 + (position2 * 0.5f) + position1);
    checkpointModelMatrix = glm::scale(checkpointModelMatrix, scale2);
    RenderSimpleMesh(meshes["cube"], shaders["CubeShader"], checkpointModelMatrix, glm::vec3(0.5f, 0.5f, 0.5f));

    // 7
    checkpointModelMatrix = glm::mat4(1);
    checkpointModelMatrix = glm::translate(checkpointModelMatrix, position + position1 + glm::vec3(0.2f, 1.0f, 0));
    checkpointModelMatrix = glm::scale(checkpointModelMatrix, scale4);
    RenderSimpleMesh(meshes["cube"], shaders["CubeShader"], checkpointModelMatrix, color);

    // 8
    checkpointModelMatrix = glm::mat4(1);
    checkpointModelMatrix = glm::translate(checkpointModelMatrix, position + position1 + position2 + glm::vec3(-0.2f, 1.0f, 0));
    checkpointModelMatrix = glm::scale(checkpointModelMatrix, scale4);
    RenderSimpleMesh(meshes["cube"], shaders["CubeShader"], checkpointModelMatrix, color);

    // 9
    checkpointModelMatrix = glm::mat4(1);
    checkpointModelMatrix = glm::translate(checkpointModelMatrix, position + position1 + glm::vec3(1.0f, 0.2f, 0));
    checkpointModelMatrix = glm::scale(checkpointModelMatrix, scale3);
    RenderSimpleMesh(meshes["cube"], shaders["CubeShader"], checkpointModelMatrix, color);

    // 10
    checkpointModelMatrix = glm::mat4(1);
    checkpointModelMatrix = glm::translate(checkpointModelMatrix, position + position1 + glm::vec3(1.0f, 1.8f, 0));
    checkpointModelMatrix = glm::scale(checkpointModelMatrix, scale3);
    RenderSimpleMesh(meshes["cube"], shaders["CubeShader"], checkpointModelMatrix, color);
}

float Tema2::Noise(float x, float z) {
    return glm::fract(sin(glm::dot(glm::vec2(x, z), glm::vec2(10.0f, 25.0f))) * 100.0f);
}

float Tema2::GetTerrainHeight(float x, float z) {
    int col = static_cast<int>(x / width);
    int row = static_cast<int>(z / height);

    if (col < 0 || col >= n || row < 0 || row >= m) {
        return 0.0f;
    }

    float frequency = 0.1f;
    float amplitude = 0.6f;

    float height = Noise(x * frequency, z * frequency) * amplitude;
    return height;
}

bool Tema2::CheckCollisionWithCylinder(const glm::vec3& dronePosition, const glm::vec3& cylinderPosition, float cylinderRadius, float cylinderHeight) {
    glm::vec2 dronePosXZ(dronePosition.x, dronePosition.z);
    glm::vec2 cylinderPosXZ(cylinderPosition.x, cylinderPosition.z);
    float distanceXZ = glm::length(dronePosXZ - cylinderPosXZ);

    if (distanceXZ < cylinderRadius && dronePosition.y > cylinderPosition.y && dronePosition.y < cylinderPosition.y + cylinderHeight) {
        return true;
    }
    return false;
}

bool Tema2::CheckCollisionWithBox(const glm::vec3& dronePosition, const glm::vec3& boxPosition, const glm::vec3& boxSize) {
    float minX = boxPosition.x - boxSize.x / 2.0f;
    float maxX = boxPosition.x + boxSize.x / 2.0f;

    float minY = boxPosition.y;
    float maxY = boxPosition.y + boxSize.y;

    float minZ = boxPosition.z - boxSize.z / 2.0f;
    float maxZ = boxPosition.z + boxSize.z / 2.0f;

    if (dronePosition.x >= minX && dronePosition.x <= maxX &&
        dronePosition.y >= minY && dronePosition.y <= maxY &&
        dronePosition.z >= minZ && dronePosition.z <= maxZ) {
        return true;
    }

    return false;
}

bool Tema2::CheckCollisionWithCone(const glm::vec3& dronePosition, const glm::vec3& conePosition, float coneRadius, float coneHeight) {
    float minY = conePosition.y;
    float maxY = conePosition.y + coneHeight;

    if (dronePosition.y < minY || dronePosition.y > maxY) {
        return false;
    }

    float heightRatio = (dronePosition.y - minY) / coneHeight;
    float currentRadius = coneRadius * (1.0f - heightRatio);

    glm::vec2 dronePosXZ(dronePosition.x, dronePosition.z);
    glm::vec2 coneBasePosXZ(conePosition.x, conePosition.z);
    float distanceXZ = glm::length(dronePosXZ - coneBasePosXZ);

    if (distanceXZ <= currentRadius) {
        return true;
    }

    return false;
}

bool Tema2::CheckCollisionWithPyramid(const glm::vec3& dronePosition, const glm::vec3& prismPosition, float baseLength, float prismHeight) {
    float halfLength = baseLength / 2.0f;

    float minX = prismPosition.x - halfLength;
    float maxX = prismPosition.x + halfLength;

    float minY = prismPosition.y;
    float maxY = prismPosition.y + prismHeight;

    float minZ = prismPosition.z - halfLength;
    float maxZ = prismPosition.z + halfLength;

    if (dronePosition.x >= minX && dronePosition.x <= maxX &&
        dronePosition.y >= minY && dronePosition.y <= maxY &&
        dronePosition.z >= minZ && dronePosition.z <= maxZ) {
        return true;
    }

    return false;
}

bool Tema2::CheckCollisionWithCheckpoint(const glm::vec3& dronePosition, const glm::vec3& checkpointPosition) {
	float outerWidth = 2.0f;  // latimea exterioara pe axa X
	float outerHeight = 4.0f; // inaltimea exterioara pe axa Y
	float depth = 0.2f;       // grosimea chenarului pe axa Z
	float marginThickness = 0.5f;  // grosimea marginii chenarului interior

    // limitele exterioare
    float outerMinX = checkpointPosition.x;
    float outerMaxX = checkpointPosition.x + outerWidth;

    float outerMinY = checkpointPosition.y;
    float outerMaxY = checkpointPosition.y + outerHeight;

    float minZ = checkpointPosition.z - depth;
    float maxZ = checkpointPosition.z + depth;

	// limitele interioare
    float innerMinX = outerMinX + marginThickness;
    float innerMaxX = outerMaxX - marginThickness;

    float innerMinY = outerMinY + marginThickness;
    float innerMaxY = outerMaxY - marginThickness;

	// margine stanga si dreapta
    bool collisionVerticalLeft = (dronePosition.x >= outerMinX && dronePosition.x <= innerMinX &&
        dronePosition.y >= outerMinY && dronePosition.y <= outerMaxY &&
        dronePosition.z >= minZ && dronePosition.z <= maxZ);

    bool collisionVerticalRight = (dronePosition.x >= innerMaxX && dronePosition.x <= outerMaxX &&
        dronePosition.y >= outerMinY && dronePosition.y <= outerMaxY &&
        dronePosition.z >= minZ && dronePosition.z <= maxZ);

	// margine jos si sus
	bool collisionHorizontalBottom = (dronePosition.y >= outerMinY + 2.0f && dronePosition.y <= innerMinY + 2.0f &&
        dronePosition.x >= outerMinX && dronePosition.x <= outerMaxX &&
        dronePosition.z >= minZ && dronePosition.z <= maxZ);

    bool collisionHorizontalTop = (dronePosition.y >= innerMaxY && dronePosition.y <= outerMaxY &&
        dronePosition.x >= outerMinX && dronePosition.x <= outerMaxX &&
        dronePosition.z >= minZ && dronePosition.z <= maxZ);

    return collisionVerticalLeft || collisionVerticalRight || collisionHorizontalBottom || collisionHorizontalTop;
}

bool Tema2::ScoreCheckpoint(const glm::vec3& dronePosition, const glm::vec3& checkpointPosition) {
    float outerWidth = 2.0f;
    float outerHeight = 4.0f;
    float depth = 0.2f;
    float marginThickness = 0.5f;

    // limitele exterioare
    float outerMinX = checkpointPosition.x;
    float outerMaxX = checkpointPosition.x + outerWidth;

    float outerMinY = checkpointPosition.y;
    float outerMaxY = checkpointPosition.y + outerHeight;

    float minZ = checkpointPosition.z - depth;
    float maxZ = checkpointPosition.z + depth;

	// limitele interioare
    float innerMinX = outerMinX + marginThickness;
    float innerMaxX = outerMaxX - marginThickness;

    float innerMinY = outerMinY + marginThickness;
    float innerMaxY = outerMaxY - marginThickness;

	// trecere prin interiorul checkpoint-ului
	bool check = (dronePosition.x >= innerMinX && dronePosition.x <= innerMaxX &&
		dronePosition.y >= innerMinY && dronePosition.y <= innerMaxY &&
		dronePosition.z >= minZ && dronePosition.z <= maxZ);

	return check;
}

void Tema2::RenderDirectionArrow(const glm::vec3& dronePosition, int score) {
    if (score < checkpointPositions.size()) {
		// pozitia checkpoint-ului verde
        glm::vec3 checkpointPosition = checkpointPositions[score];

		// vectorul directie de la drona la checkpoint
        glm::vec3 direction = glm::normalize(checkpointPosition - dronePosition);

		// calculam unghiul de rotatie pentru sageata
		float angle = atan2(direction.x, direction.z); // folosim atan2 pentru a obtine unghiul in radiani

        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, dronePosition + glm::vec3(0, 1, 0));
        modelMatrix = glm::rotate(modelMatrix, angle, glm::vec3(0, 1, 0));
		modelMatrix = glm::rotate(modelMatrix, glm::radians(30.0f), glm::vec3(1, 0, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(1.5f, 1.0f, 1.5f));

        RenderSimpleMesh(meshes["arrow"], shaders["VertexColor"], modelMatrix, glm::vec3(1, 1, 0));
    }
}

void Tema2::OnInputUpdate(float deltaTime, int mods) {
    if (window->KeyHold(GLFW_KEY_LEFT)) {
        droneYaw += 50.0f * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_RIGHT)) {
        droneYaw -= 50.0f * deltaTime;
    }

    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(droneYaw), glm::vec3(0, 1, 0));
    glm::vec3 forward = glm::vec3(droneSpeed * rotationMatrix * glm::vec4(0, 0, -1, 0));
    glm::vec3 right = glm::vec3(droneSpeed * rotationMatrix * glm::vec4(1, 0, 0, 0));

	glm::vec3 nextPosition = dronePosition;

    nextPosition.x = glm::clamp(nextPosition.x, minX, maxX);
    nextPosition.z = glm::clamp(nextPosition.z, minZ, maxZ);
    nextPosition.y = glm::max(nextPosition.y, GetTerrainHeight(nextPosition.x, nextPosition.z));

    if (window->KeyHold(GLFW_KEY_W) && collision != 1) {
        nextPosition += forward * deltaTime;
		key = 1;
    }
    if (window->KeyHold(GLFW_KEY_S) && collision != 2) {
        nextPosition -= forward * deltaTime;
		key = 2;
    }
    if (window->KeyHold(GLFW_KEY_A) && collision != 3) {
        nextPosition -= right * deltaTime;
		key = 3;
    }
    if (window->KeyHold(GLFW_KEY_D) && collision != 4) {
        nextPosition += right * deltaTime;
		key = 4;
    }
    if (window->KeyHold(GLFW_KEY_E)) {
        nextPosition.y += deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_Q)) {
        nextPosition.y -= deltaTime;
    }
	if (window->KeyHold(GLFW_KEY_LEFT_SHIFT)) {
		droneSpeed = 5.0f;
	}
	else {
		droneSpeed = 2.0f;
	}

	// coleziune cu copacii
    collision = 0;
    for (const auto& tree : treePositions) {
        if (CheckCollisionWithCylinder(nextPosition, tree, 0.1f, 0.5f)) {
            collision = key;
        }
		if (CheckCollisionWithCone(nextPosition, tree + glm::vec3(0, 0.5f, 0), 0.5f, 0.5f)) {
			collision = key;
		}
    }
	for (const auto& building : buildingPositions) {
        // coliziune cu baza cladirilor
		if (CheckCollisionWithBox(nextPosition, building, glm::vec3(1.1f, 2.0f, 1.1f))) {
			collision = key;
		}
        // coleziune cu varful cladirilor
		if (CheckCollisionWithPyramid(nextPosition, building + glm::vec3(0, 2.0f, 0), 2.0f, 0.5f)) {
			collision = key;
		}
	}
    // coleziune cu checkpoints
    for (const auto& check : checkpointPositions) {
        if (CheckCollisionWithCheckpoint(nextPosition, check)) {
            collision = key;
        }
    }

	// trecere prin checkpoint cu  chenarul verde
	if (ScoreCheckpoint(nextPosition, checkpointPositions[score])) {
		score++;
	}

    dronePosition = nextPosition;

    // actualizeaza pozitia camerei folosind camera_mea
    glm::vec3 cameraOffset = glm::vec3(0.0f, 0.3f, 0.0f);
    camera->position = dronePosition + cameraOffset;
    camera->forward = forward;
    camera->right = right;

    // seteaza camera
    GetSceneCamera()->SetPosition(camera->position);
    GetSceneCamera()->SetRotation(glm::quatLookAt(glm::normalize(camera->forward), glm::vec3(0, 1, 0)));

    glm::vec3 minimapCameraPosition = glm::vec3(dronePosition.x, 50.0f, dronePosition.z);
    glm::vec3 minimapCameraTarget = dronePosition;
    glm::vec3 minimapUp = glm::vec3(0, 1, 0);

    minimapCamera->Set(minimapCameraPosition, minimapCameraTarget, minimapUp);

    glm::mat4 minimapViewMatrix = glm::lookAt(minimapCameraPosition, minimapCameraTarget, minimapUp);
    glm::mat4 minimapProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, 0.1f, 100.0f);
}

void Tema2::OnKeyPress(int key, int mods){
}

void Tema2::OnKeyRelease(int key, int mods){
}

void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY){ 
}

void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods){
}

void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods){
}

void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY){
}

void Tema2::OnWindowResize(int width, int height){
}
