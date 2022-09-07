#ifndef ENTITY_H
#define ENTITY_H

#include <glm/glm.hpp> //glm::mat4
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <list> //std::list
#include <array> //std::array
#include <memory> //std::unique_ptr

class Transform
{
protected:
	//Local space information
	glm::vec3 m_pos = { 0.0f, 0.0f, 0.0f };
	glm::vec3 m_eulerRot = { 0.0f, 0.0f, 0.0f }; //In degrees
	glm::vec3 m_scale = { 1.0f, 1.0f, 1.0f };

	//Global space informaiton concatenate in matrix
	glm::mat4 m_modelMatrix = glm::mat4(1.0f);

	//Dirty flag
	bool m_isDirty = true;

protected:
	glm::mat4 getLocalModelMatrix()
	{
		const glm::mat4 transformX = glm::rotate(glm::mat4(1.0f), glm::radians(m_eulerRot.x), glm::vec3(1.0f, 0.0f, 0.0f));
		const glm::mat4 transformY = glm::rotate(glm::mat4(1.0f), glm::radians(m_eulerRot.y), glm::vec3(0.0f, 1.0f, 0.0f));
		const glm::mat4 transformZ = glm::rotate(glm::mat4(1.0f), glm::radians(m_eulerRot.z), glm::vec3(0.0f, 0.0f, 1.0f));

		// Y * X * Z
		const glm::mat4 roationMatrix = transformY * transformX * transformZ;

		// translation * rotation * scale (also know as TRS matrix)
		return glm::translate(glm::mat4(1.0f), m_pos) * roationMatrix * glm::scale(glm::mat4(1.0f), m_scale);
	}
public:

	void computeModelMatrix()
	{
		m_modelMatrix = getLocalModelMatrix();
	}

	void computeModelMatrix(const glm::mat4& parentGlobalModelMatrix)
	{
		m_modelMatrix = parentGlobalModelMatrix * getLocalModelMatrix();
	}

	void setLocalPosition(const glm::vec3& newPosition)
	{
		m_pos = newPosition;
		m_isDirty = true;
	}

	void setLocalRotation(const glm::vec3& newRotation)
	{
		m_eulerRot = newRotation;
		m_isDirty = true;
	}

	void setLocalScale(const glm::vec3& newScale)
	{
		m_scale = newScale;
		m_isDirty = true;
	}

	const glm::vec3& getGlobalPosition() const
	{
		return m_modelMatrix[3];
	}

	const glm::vec3& getLocalPosition() const
	{
		return m_pos;
	}

	const glm::vec3& getLocalRotation() const
	{
		return m_eulerRot;
	}

	const glm::vec3& getLocalScale() const
	{
		return m_scale;
	}

	glm::mat4 getTranslation() const {
		glm::mat4 rotation = glm::toMat4(glm::quat(glm::radians(m_eulerRot)));

		return glm::translate( glm::mat4(1.0), (glm::vec3)getGlobalPosition() ) 
			* rotation 
			* glm::scale(glm::mat4(1.0f), m_scale);
	}

	const glm::mat4& getModelMatrix() const
	{
		return m_modelMatrix;
	}

	glm::vec3 getRight() const
	{
		return m_modelMatrix[0];
	}


	glm::vec3 getUp() const
	{
		return m_modelMatrix[1];
	}

	glm::vec3 getBackward() const
	{
		return m_modelMatrix[2];
	}

	glm::vec3 getForward() const
	{
		return -m_modelMatrix[2];
	}

	glm::vec3 getGlobalScale() const
	{
		return { glm::length(getRight()), glm::length(getUp()), glm::length(getBackward()) };
	}

	bool isDirty() const
	{
		return m_isDirty;
	}
};

struct Plan
{
	glm::vec3 normal = { 0.f, 1.f, 0.f }; // unit vector
	float     distance = 0.f;        // Distance with origin

	Plan() = default;

	Plan(const glm::vec3& p1, const glm::vec3& norm)
		: normal(glm::normalize(norm)),
		distance(glm::dot(normal, p1))
	{}

	float getSignedDistanceToPlan(const glm::vec3& point) const
	{
		return glm::dot(normal, point) - distance;
	}
};

struct Frustum
{
	Plan topFace;
	Plan bottomFace;

	Plan rightFace;
	Plan leftFace;

	Plan farFace;
	Plan nearFace;
};

struct BoundingVolume
{
	virtual bool isOnFrustum(const Frustum& camFrustum, const Transform& transform) const = 0;

	virtual bool isOnOrForwardPlan(const Plan& plan) const = 0;

	bool isOnFrustum(const Frustum& camFrustum) const
	{
		return (isOnOrForwardPlan(camFrustum.leftFace) &&
			isOnOrForwardPlan(camFrustum.rightFace) &&
			isOnOrForwardPlan(camFrustum.topFace) &&
			isOnOrForwardPlan(camFrustum.bottomFace) &&
			isOnOrForwardPlan(camFrustum.nearFace) &&
			isOnOrForwardPlan(camFrustum.farFace));
	};
};

struct Sphere : public BoundingVolume
{
	glm::vec3 center{ 0.f, 0.f, 0.f };
	float radius{ 0.f };

	Sphere(const glm::vec3& inCenter, float inRadius)
		: BoundingVolume{}, center{ inCenter }, radius{ inRadius }
	{}

	bool isOnOrForwardPlan(const Plan& plan) const final
	{
		return plan.getSignedDistanceToPlan(center) > -radius;
	}

	bool isOnFrustum(const Frustum& camFrustum, const Transform& transform) const final
	{
		//Get global scale thanks to our transform
		const glm::vec3 globalScale = transform.getGlobalScale();

		//Get our global center with process it with the global model matrix of our transform
		const glm::vec3 globalCenter{ transform.getModelMatrix() * glm::vec4(center, 1.f) };

		//To wrap correctly our shape, we need the maximum scale scalar.
		const float maxScale = std::max(std::max(globalScale.x, globalScale.y), globalScale.z);

		//Max scale is assuming for the diameter. So, we need the half to apply it to our radius
		Sphere globalSphere(globalCenter, radius * (maxScale * 0.5f));

		//Check Firstly the result that have the most chance to faillure to avoid to call all functions.
		return (globalSphere.isOnOrForwardPlan(camFrustum.leftFace) &&
			globalSphere.isOnOrForwardPlan(camFrustum.rightFace) &&
			globalSphere.isOnOrForwardPlan(camFrustum.farFace) &&
			globalSphere.isOnOrForwardPlan(camFrustum.nearFace) &&
			globalSphere.isOnOrForwardPlan(camFrustum.topFace) &&
			globalSphere.isOnOrForwardPlan(camFrustum.bottomFace));
	};
};

struct SquareAABB : public BoundingVolume
{
	glm::vec3 center{ 0.f, 0.f, 0.f };
	float extent{ 0.f };

	SquareAABB(const glm::vec3& inCenter, float inExtent)
		: BoundingVolume{}, center{ inCenter }, extent{ inExtent }
	{}

	bool isOnOrForwardPlan(const Plan& plan) const final
	{
		// Compute the projection interval radius of b onto L(t) = b.c + t * p.n
		const float r = extent * (std::abs(plan.normal.x) + std::abs(plan.normal.y) + std::abs(plan.normal.z));
		return -r <= plan.getSignedDistanceToPlan(center);
	}

	bool isOnFrustum(const Frustum& camFrustum, const Transform& transform) const final
	{
		//Get global scale thanks to our transform
		const glm::vec3 globalCenter{ transform.getModelMatrix() * glm::vec4(center, 1.f) };

		// Scaled orientation
		const glm::vec3 right = transform.getRight() * extent;
		const glm::vec3 up = transform.getUp() * extent;
		const glm::vec3 forward = transform.getForward() * extent;

		const float newIi = std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, right)) +
			std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, up)) +
			std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, forward));

		const float newIj = std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, right)) +
			std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, up)) +
			std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, forward));

		const float newIk = std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, right)) +
			std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, up)) +
			std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, forward));

		const SquareAABB globalAABB(globalCenter, std::max(std::max(newIi, newIj), newIk));

		return (globalAABB.isOnOrForwardPlan(camFrustum.leftFace) &&
			globalAABB.isOnOrForwardPlan(camFrustum.rightFace) &&
			globalAABB.isOnOrForwardPlan(camFrustum.topFace) &&
			globalAABB.isOnOrForwardPlan(camFrustum.bottomFace) &&
			globalAABB.isOnOrForwardPlan(camFrustum.nearFace) &&
			globalAABB.isOnOrForwardPlan(camFrustum.farFace));
	};
};

struct AABB : public BoundingVolume
{
	glm::vec3 center{ 0.f, 0.f, 0.f };
	glm::vec3 extents{ 0.f, 0.f, 0.f };

	AABB(const glm::vec3& min, const glm::vec3& max)
		: BoundingVolume{}, center{ (max + min) * 0.5f }, extents{ max.x - center.x, max.y - center.y, max.z - center.z }
	{}

	AABB(const glm::vec3& inCenter, float iI, float iJ, float iK)
		: BoundingVolume{}, center{ inCenter }, extents{ iI, iJ, iK }
	{}

	std::array<glm::vec3, 8> getVertice() const
	{
		std::array<glm::vec3, 8> vertice;
		vertice[0] = { center.x - extents.x, center.y - extents.y, center.z - extents.z };
		vertice[1] = { center.x + extents.x, center.y - extents.y, center.z - extents.z };
		vertice[2] = { center.x - extents.x, center.y + extents.y, center.z - extents.z };
		vertice[3] = { center.x + extents.x, center.y + extents.y, center.z - extents.z };
		vertice[4] = { center.x - extents.x, center.y - extents.y, center.z + extents.z };
		vertice[5] = { center.x + extents.x, center.y - extents.y, center.z + extents.z };
		vertice[6] = { center.x - extents.x, center.y + extents.y, center.z + extents.z };
		vertice[7] = { center.x + extents.x, center.y + extents.y, center.z + extents.z };
		return vertice;
	}

	//see https://gdbooks.gitbooks.io/3dcollisions/content/Chapter2/static_aabb_plan.html
	bool isOnOrForwardPlan(const Plan& plan) const final
	{
		// Compute the projection interval radius of b onto L(t) = b.c + t * p.n
		const float r = extents.x * std::abs(plan.normal.x) + extents.y * std::abs(plan.normal.y) +
			extents.z * std::abs(plan.normal.z);

		return -r <= plan.getSignedDistanceToPlan(center);
	}

	bool isOnFrustum(const Frustum& camFrustum, const Transform& transform) const final
	{
		//Get global scale thanks to our transform
		const glm::vec3 globalCenter{ transform.getModelMatrix() * glm::vec4(center, 1.f) };

		// Scaled orientation
		const glm::vec3 right = transform.getRight() * extents.x;
		const glm::vec3 up = transform.getUp() * extents.y;
		const glm::vec3 forward = transform.getForward() * extents.z;

		const float newIi = std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, right)) +
			std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, up)) +
			std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, forward));

		const float newIj = std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, right)) +
			std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, up)) +
			std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, forward));

		const float newIk = std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, right)) +
			std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, up)) +
			std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, forward));

		const AABB globalAABB(globalCenter, newIi, newIj, newIk);

		return (globalAABB.isOnOrForwardPlan(camFrustum.leftFace) &&
			globalAABB.isOnOrForwardPlan(camFrustum.rightFace) &&
			globalAABB.isOnOrForwardPlan(camFrustum.topFace) &&
			globalAABB.isOnOrForwardPlan(camFrustum.bottomFace) &&
			globalAABB.isOnOrForwardPlan(camFrustum.nearFace) &&
			globalAABB.isOnOrForwardPlan(camFrustum.farFace));
	};
};

Frustum createFrustumFromCamera(const Camera& cam, float aspect, float fovY, float zNear, float zFar)
{
	Frustum     frustum;
	const float halfVSide = zFar * tanf(fovY * .5f);
	const float halfHSide = halfVSide * aspect;
	const glm::vec3 frontMultFar = zFar * cam.Front;

	frustum.nearFace = { cam.Position + zNear * cam.Front, cam.Front };
	frustum.farFace = { cam.Position + frontMultFar, -cam.Front };
	frustum.rightFace = { cam.Position, glm::cross(cam.Up, frontMultFar + cam.Right * halfHSide) };
	frustum.leftFace = { cam.Position, glm::cross(frontMultFar - cam.Right * halfHSide, cam.Up) };
	frustum.topFace = { cam.Position, glm::cross(cam.Right, frontMultFar - cam.Up * halfVSide) };
	frustum.bottomFace = { cam.Position, glm::cross(frontMultFar + cam.Up * halfVSide, cam.Right) };

	return frustum;
}

AABB generateAABB(const Model& model)
{
	glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
	glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::min());
	for (auto&& mesh : model.meshes)
	{
		for (auto&& vertex : mesh.vertices)
		{
			minAABB.x = std::min(minAABB.x, vertex.Position.x);
			minAABB.y = std::min(minAABB.y, vertex.Position.y);
			minAABB.z = std::min(minAABB.z, vertex.Position.z);

			maxAABB.x = std::max(maxAABB.x, vertex.Position.x);
			maxAABB.y = std::max(maxAABB.y, vertex.Position.y);
			maxAABB.z = std::max(maxAABB.z, vertex.Position.z);
		}
	}
	return AABB(minAABB, maxAABB);
}

Sphere generateSphereBV(const Model& model)
{
	glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
	glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::min());
	for (auto&& mesh : model.meshes)
	{
		for (auto&& vertex : mesh.vertices)
		{
			minAABB.x = std::min(minAABB.x, vertex.Position.x);
			minAABB.y = std::min(minAABB.y, vertex.Position.y);
			minAABB.z = std::min(minAABB.z, vertex.Position.z);

			maxAABB.x = std::max(maxAABB.x, vertex.Position.x);
			maxAABB.y = std::max(maxAABB.y, vertex.Position.y);
			maxAABB.z = std::max(maxAABB.z, vertex.Position.z);
		}
	}

	return Sphere((maxAABB + minAABB) * 0.5f, glm::length(minAABB - maxAABB));
}

struct LightEntity 
{
	bool isEntityLight = false;
	glm::vec4 color = glm::vec4(2.0f);
	float radius = 10.0f;
};

class Entity
{
public:
	//Scene graph
	int id;
	std::list<std::unique_ptr<Entity>> children;
	Entity* parent = nullptr;
	static int counter;
	LightEntity pointLight;

	//Space information
	Transform transform;

	Model* pModel = nullptr;
	std::unique_ptr<AABB> boundingVolume;

	char entityName[128] = "New Element";

	Entity() {
		id = counter + 1;
		counter++;
	}

	Entity(const char* name) {
		id = counter + 1;
		counter++;
		strcpy(entityName, name);
	}

	// constructor, expects a filepath to a 3D model.
	Entity(Model& model) : pModel{ &model }
	{
		id = counter + 1;
		counter++;
		boundingVolume = std::make_unique<AABB>(generateAABB(model));
		//boundingVolume = std::make_unique<Sphere>(generateSphereBV(model));
	}

	// constructor, expects a filepath to a 3D model.
	Entity(Model& model, const char* name) : pModel{ &model }
	{
		id = counter + 1;
		counter++;
		strcpy(entityName, name);
		boundingVolume = std::make_unique<AABB>(generateAABB(model));
		//boundingVolume = std::make_unique<Sphere>(generateSphereBV(model));
	}

	Entity(bool isEntityLight, const char* name){
		pointLight.isEntityLight = isEntityLight;
		id = counter + 1;
		counter++;
		strcpy(entityName, name);
	}

	// This is needed by list.remove()
	bool operator == (const Entity& e) const { return id == e.id; }
	bool operator != (const Entity& e) const { return !operator==(e); }

	AABB getGlobalAABB()
	{
		//Get global scale thanks to our transform
		const glm::vec3 globalCenter{ transform.getModelMatrix() * glm::vec4(boundingVolume->center, 1.f) };

		// Scaled orientation
		const glm::vec3 right = transform.getRight() * boundingVolume->extents.x;
		const glm::vec3 up = transform.getUp() * boundingVolume->extents.y;
		const glm::vec3 forward = transform.getForward() * boundingVolume->extents.z;

		const float newIi = std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, right)) +
			std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, up)) +
			std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, forward));

		const float newIj = std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, right)) +
			std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, up)) +
			std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, forward));

		const float newIk = std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, right)) +
			std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, up)) +
			std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, forward));

		return AABB(globalCenter, newIi, newIj, newIk);
	}

	//Add child. Argument input is argument of any constructor that you create. By default you can use the default constructor and don't put argument input.
	template<typename... TArgs>
	void addChild(TArgs&... args)
	{
		children.emplace_back(std::make_unique<Entity>(args...));
		children.back()->parent = this;
	}

	template<typename... TArgs>
	void addChildFront(TArgs&... args)
	{
		children.emplace_front(std::make_unique<Entity>(args...));
		children.back()->parent = this;
	}

	void addChild(bool isEntityLight, const char* name)
	{
		children.emplace_back(std::make_unique<Entity>(isEntityLight, name));
		children.back()->parent = this;
	}

	//Update transform if it was changed
	void updateSelfAndChild()
	{
		if (!transform.isDirty())
			return;

		forceUpdateSelfAndChild();
	}

	//Force update of transform even if local space don't change
	void forceUpdateSelfAndChild()
	{
		if (parent)
			transform.computeModelMatrix(parent->transform.getModelMatrix());
		else
			transform.computeModelMatrix();

		for (auto&& child : children)
		{
			child->forceUpdateSelfAndChild();
		}
	}


	void drawSelfAndChild(const Frustum& frustum, Shader& ourShader, unsigned int& display, unsigned int& total)
	{
		total++;
		for (auto&& child : children)
		{
			child->drawSelfAndChild(frustum, ourShader, display, total);
		}

		if (pModel == nullptr)
			return;

		if (boundingVolume->isOnFrustum(frustum, transform))
		{
			ourShader.setMat4("model", transform.getModelMatrix());
			pModel->Draw(ourShader);
			display++;
		}
	}

	void drawPointLights(Shader& shader, unsigned int& total, Camera& camera)
	{
		for (auto&& child : children)
		{
			child->drawPointLights(shader, total, camera);
		}

		if (pModel == nullptr && pointLight.isEntityLight) {
			glm::vec3 lightPositionViewSpace = glm::vec3(camera.GetViewMatrix() * glm::vec4(transform.getGlobalPosition(), 1.0f));

			glUniform3f(glGetUniformLocation(shader.ID, ("lightPointArray[" + std::to_string(total) + "].position").c_str()), lightPositionViewSpace.x, lightPositionViewSpace.y, lightPositionViewSpace.z);
			glUniform4f(glGetUniformLocation(shader.ID, ("lightPointArray[" + std::to_string(total) + "].color").c_str()), pointLight.color.x, pointLight.color.y, pointLight.color.z, pointLight.color.w);
			glUniform1f(glGetUniformLocation(shader.ID, ("lightPointArray[" + std::to_string(total) + "].radius").c_str()), pointLight.radius);
			total++;
		}
	}

	int getTotalChildren() {
		return countTotalEntityChildren(*this);
	}

	private: int countTotalEntityChildren(Entity& entity) {
		int countChildrenEntities = entity.children.size();
		int totalEntities = 0;

		if (countChildrenEntities <= 0)
			return 0;

		// 'it' gives the address to the pointer pointing at the first/last child of the list
		for (auto it = entity.children.begin(); it != entity.children.end(); ++it)
		{
			totalEntities += countTotalEntityChildren(**it);
			totalEntities++;
		}
		return totalEntities;
	}
};

int Entity::counter = 0;
#endif
