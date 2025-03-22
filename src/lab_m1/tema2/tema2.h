#pragma once

#include "components/simple_scene.h"
#include "lab_m1/tema2/lab_camera.h"


namespace m1
{
    class Tema2 : public gfxc::SimpleScene
    {
     public:
        Tema2();
        ~Tema2();

        struct ViewportArea
        {
            ViewportArea() : x(0), y(0), width(1), height(1) {}
            ViewportArea(int x, int y, int width, int height)
                : x(x), y(y), width(width), height(height) {}
            int x;
            int y;
            int width;
            int height;
        };

        void Init() override;

        Mesh* CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned int>& indices);


     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color);

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

        void Tema2::RenderScene(float deltaTimeSeconds);
        void Tema2::CreateTerrain();

        void Tema2::GenerateCylinder(std::vector<VertexFormat>& vertices, std::vector<unsigned int>& indices, float radius, float height, int segments);
        void Tema2::GenerateCone(std::vector<VertexFormat>& vertices, std::vector<unsigned int>& indices, float radius, float height, int segments);
        void Tema2::CreateTree(const glm::vec3& position, const glm::vec3& scale_trunk, const glm::vec3& scale_foliage);

        void Tema2::GenerateCube(std::vector<VertexFormat>& vertices, std::vector<unsigned int>& indices, float side);
        void Tema2::GeneratePyramid(std::vector<VertexFormat>& vertices, std::vector<unsigned int>& indices, float baseLength, float height);
        void Tema2::CreateBuilding(const glm::vec3& position, const glm::vec3& scale_base, const glm::vec3& scale_roof);

        void Tema2::CreateDrone(const glm::vec3& position, const glm::vec3& scale_body, const glm::vec3& scale_connector, const glm::vec3& scale_propeller, float deltaTime);

        void Tema2::CreateCheckPoint(const glm::vec3& position, glm::vec3 color);


        float Tema2::Noise(float x, float z);
        float Tema2::GetTerrainHeight(float x, float z);
        bool Tema2::CheckCollisionWithCylinder(const glm::vec3& dronePosition, const glm::vec3& cylinderPosition, float cylinderRadius, float cylinderHeight);
        bool Tema2::CheckCollisionWithBox(const glm::vec3& dronePosition, const glm::vec3& boxPosition, const glm::vec3& boxSize);
        bool Tema2::CheckCollisionWithCone(const glm::vec3& dronePosition, const glm::vec3& conePosition, float coneRadius, float coneHeight);
        bool Tema2::CheckCollisionWithPyramid(const glm::vec3& dronePosition, const glm::vec3& prismPosition, float baseLength, float prismHeight);
        bool Tema2::CheckCollisionWithCheckpoint(const glm::vec3& dronePosition, const glm::vec3& checkpointPosition);

        bool Tema2::ScoreCheckpoint(const glm::vec3& dronePosition, const glm::vec3& checkpointPosition);
        void Tema2::RenderDirectionArrow(const glm::vec3& dronePosition, int score);
        Mesh* Tema2::CreateArrow(const std::string& name, const glm::vec3& color);


     protected:
        implemented::Camera_mea *camera;
        implemented::Camera_mea * minimapCamera;
        glm::mat4 projectionMatrix;
        bool renderCameraTarget;

		// terrain
		float n, m, width, height;
		std::vector<VertexFormat> vertices;
		std::vector<unsigned int> indices;
		float minX, maxX, minZ, maxZ;

		// trees
        std::vector<glm::vec3> treePositions;

		// buildings
		std::vector<glm::vec3> buildingPositions;

		// checkpoints
		std::vector<glm::vec3> checkpointPositions;
        int nr_checkpoints = 10;
        int score = 0;

        // drona
        glm::vec3 dronePosition;
        glm::vec3 droneBodyScale;
        glm::vec3 droneConnectorScale;
		glm::vec3 dronePropellerScale;
        float propellerAngle;
        float propellerSpeed;
        float droneSpeed;
        float droneYaw;

        GLenum polygonMode;
        ViewportArea miniViewportArea;

        int collision;
        int key;
    };
}   // namespace m1
