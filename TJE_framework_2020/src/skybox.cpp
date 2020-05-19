#include "skybox.h"

SkyBox::SkyBox() 
{
    skyShader = Shader::Get("data/shaders/sky.vs", "data/shaders/sky.fs");
    //skyShader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
    //loadCubeMap(TEXTURE_FILES);
    texture = new Texture();
    cube = Mesh::Get("data/box.ASE");

    img1 = new Image();
    img1->loadTGA("data/sky/right.tga");
    data_files[0] = img1->data;

    img2 = new Image();
    img2->loadTGA("data/sky/left.tga");
    data_files[1] = img2->data;

    img3 = new Image();
    img3->loadTGA("data/sky/top.tga");
    data_files[2] = img3->data;

    img4 = new Image();
    img4->loadTGA("data/sky/bottom.tga");
    data_files[3] = img4->data;

    img5 = new Image();
    img5->loadTGA("data/sky/back.tga");
    data_files[4] = img5->data;

    img6 = new Image();
    img6->loadTGA("data/sky/front.tga");
    data_files[5] = img6->data;
}
