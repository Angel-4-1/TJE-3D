#include "tree.h"
#include "shader.h"
#include "camera.h"
#include "mesh.h"
#include "gamemap.h"
#include "scene.h"

Tree::Tree()
{
	trees_type = eType::TREE4;
	vertices.clear();
}

void Tree::render()
{
	/**Render the trees using instancing**/
	if (vertices.size() == 0)
		return;

	Shader* shader_instanced = Shader::Get("data/shaders/instanced.vs", "data/shaders/phong.fs");
	Camera* camera = Camera::current;

	//enable shader
	shader_instanced->enable();

	Scene::getInstance()->light->uploadToShader(shader_instanced);

	//upload uniforms
	shader_instanced->setUniform("u_material_shininess", 30.0f);
	shader_instanced->setUniform("u_light_direction", Vector3(0.3, 0.6, 0.2).normalize());

	//upload uniforms
	shader_instanced->setUniform("u_color", Vector4(1, 1, 1, 1));
	shader_instanced->setUniform("u_viewprojection", camera->viewprojection_matrix);
	shader_instanced->setUniform("u_texture", GameMap::getInstance()->prototypes[(int)trees_type].texture, 0);
	shader_instanced->setUniform("u_camera_pos", camera->eye);

	Mesh* mesh = GameMap::getInstance()->prototypes[(int)trees_type].mesh;
	mesh->renderInstanced(GL_TRIANGLES, &vertices[0], vertices.size());

	//disable shader
	shader_instanced->disable();
}

eType Tree::getType()
{
	return trees_type;
}

void Tree::addTree(Matrix44 tree)
{
	vertices.push_back(tree);
}
