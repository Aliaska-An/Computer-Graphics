#include "scene.h"
#include <QtMath>

Scene::Scene(QWidget* parent)
    :QOpenGLWidget(parent)
{
    a=new Axes;
    objs=new Sphere_and_conus;

    defCam();
    persp=true;

    angle_x=0;
    angle_y=0;
    angle_z=0;

    tr_x=0;
    tr_y=0;
    tr_z=0;

    sc_x=1;
    sc_y=1;
    sc_z=1;

    face=true;
    axes=true;
    fill=false;
    mat_flag=true;

    type=GL_POLYGON;

    deep=9;

    l={{5.0f,5.0f,5.0f,0.0f},{0.4f,0.4f,0.4f},{1.0f,1.0f,1.0f},{1.9f,1.9f,1.9f}};
    glnc={{1.9f,1.9f,1.9f},{0.9f,0.9f,0.9f},{5.0f,5.0f,6.0f},256.0f};
    mat={{1.1f,1.1f,1.1f},{0.5f,0.5f,0.5f},{0.3f,0.3f,0.3f},8.0f};
}

void Scene::setProject(bool f)
{
    this->persp=f;
    update();
}

void Scene::setAngleX(GLfloat angle)
{
    this->angle_x=angle;
    update();
}

void Scene::setAngleY(GLfloat angle)
{
    this->angle_y=angle;
    update();
}

void Scene::setAngleZ(GLfloat angle)
{
    this->angle_z=angle;
    update();
}

void Scene::setFace(bool f)
{
    this->face=f;
    update();
}

void Scene::setAxes(bool f)
{
    this->axes=f;
    update();
}

void Scene::setScX(GLfloat sc)
{
    this->sc_x=sc;
    update();
}

void Scene::setScY(GLfloat sc)
{
    this->sc_y=sc;
    update();
}

void Scene::setScZ(GLfloat sc)
{
    this->sc_z=sc;
    update();
}

void Scene::setTrX(GLfloat tr)
{
    this->tr_x=tr;
    update();
}

void Scene::setTrY(GLfloat tr)
{
    this->tr_y=tr;
    update();
}

void Scene::setTrZ(GLfloat tr)
{
    this->tr_z=tr;
    update();
}

void Scene::setFill(bool f)
{
    this->fill=f;
    update();
}


void Scene::setLight(float lght)
{
    l.la={lght,lght,lght};
    update();
}

void Scene::setType(GLenum type)
{
    this->type=type;
    update();
}

void Scene::setDeep(int d)
{
    this->deep=d;
    update();
}

void Scene::setMat(bool f)
{
    this->mat_flag=f;
    update();
}

void Scene::setSphereResX(int res)
{
    this->objs->setSphereResX(res);
    update();
}

void Scene::setSphereResY(int res)
{
    this->objs->setSphereResY(res);
    update();
}

void Scene::setConusRes(int res)
{
    this->objs->setConusRes(res);
    update();
}

void Scene::setCamXY()
{
    this->cam_angle=90.0f;
    this->cam_y=0.0f;
    this->cam_r=4.0f;
    update();

}

void Scene::setCamXZ()
{
    this->cam_angle=0.0f;
    this->cam_r=0.0001f;
    this->cam_y=4.0f;
}

void Scene::setCamYZ()
{
    this->cam_angle=0.0f;
    this->cam_y=0.0f;
    this->cam_r=4.0f;
    update();
}





void Scene::initializeGL(){
    initializeOpenGLFunctions();
    glClearColor(0.9f,0.9f,0.9f,0.3f);


    def_sh=new QOpenGLShaderProgram;
    def_sh->addShaderFromSourceFile(QOpenGLShader::Vertex,":/vShader.glsl");
    def_sh->addShaderFromSourceFile(QOpenGLShader::Fragment,":/fShader.glsl");
    def_sh->link();

    spline_sh=new QOpenGLShaderProgram;
    spline_sh->addShaderFromSourceFile(QOpenGLShader::Vertex,":/vShader_spline.glsl");
    spline_sh->addShaderFromSourceFile(QOpenGLShader::Fragment,":/fShader_spline.glsl");
    spline_sh->link();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}

void Scene::resizeGL(int w, int h){
    glViewport(0,0,w,h);

}

void Scene::paintGL(){

    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, width() * retinaScale, height() * retinaScale);
    glClear(GL_COLOR_BUFFER_BIT);

    face ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);

    glCullFace(GL_FRONT);

    QMatrix4x4 matrix, projection;
    QVector3D cam={this->cam_r*cosf(qDegreesToRadians(this->cam_angle)),this->cam_y,this->cam_r*sinf(qDegreesToRadians(this->cam_angle))};
    Material m=mat_flag ? glnc : mat;

    if(persp)
        projection.perspective(70.0f, 2300.0f/1080.0f, 0.1f, 100.0f);
    else
        projection.ortho(-8,8,-4,4,-20,20);



    matrix.lookAt(cam,{0.0f,1.5f,0.0f},{0,1,0});
    //matrix.rotate(100.0f * m_frame / 300, 0, 1, 0);

    def_sh->bind();
    def_sh->setUniformValue("matrix",projection*matrix);
    def_sh->setUniformValue("normal_m",matrix.normalMatrix());
    def_sh->setUniformValue("modelview",matrix);
    def_sh->setUniformValue("LightInfo.position",l.light_pos);
    def_sh->setUniformValue("LightInfo.la",l.la);
    def_sh->setUniformValue("LightInfo.ld",l.ld);
    def_sh->setUniformValue("LightInfo.ls",l.ls);
    def_sh->setUniformValue("MaterialInfo.ka",m.ka);
    def_sh->setUniformValue("MaterialInfo.kd",m.kd);
    def_sh->setUniformValue("MaterialInfo.ks",m.ks);
    def_sh->setUniformValue("MaterialInfo.Shininess",m.Shininess);
    def_sh->release();


    if(axes) a->drawAxis(def_sh);

    matrix.setToIdentity();

    matrix.lookAt(cam,{0.0f,1.5f,0.0f},{0,1,0});


    matrix.translate(tr_x,tr_y,tr_z);
    matrix.rotate(angle_x,1,0,0);
    matrix.rotate(angle_y,0,1,0);
    matrix.rotate(angle_z,0,0,1);
    matrix.scale({sc_x,sc_y,sc_z});


    def_sh->bind();
    def_sh->setUniformValue("matrix",projection*matrix);
    def_sh->setUniformValue("normal_m",matrix.normalMatrix());
    def_sh->setUniformValue("modelview",matrix);
    def_sh->setUniformValue("l.position",l.light_pos);
    def_sh->setUniformValue("l.la",l.la);
    def_sh->setUniformValue("l.ld",l.ld);
    def_sh->setUniformValue("l.ls",l.ls);
    def_sh->setUniformValue("material.ka",m.ka);
    def_sh->setUniformValue("material.kd",m.kd);
    def_sh->setUniformValue("material.ks",m.ks);
    def_sh->setUniformValue("material.Shininess",m.Shininess);
    def_sh->release();


  // dode->drawDode(def_sh,fill ? GL_FILL : GL_LINE, type, projection*matrix, 1);
    objs->drawObj(def_sh, fill ? GL_FILL : GL_LINE);
    ++m_frame;


}

void Scene::wheelEvent(QWheelEvent *event)
{
    this->cam_r-=event->delta()/500.0f;
    if(this->cam_r<0)this->cam_r+=event->delta()/500.0f;
    update();
}




void Scene::mousePressEvent(QMouseEvent *event){

    start=QPointF(event->x(),event->y());
    if(event->button()==Qt::LeftButton)mouse_flag=true;
    else if(event->button()==Qt::RightButton)mouse_flag=false;
}

void Scene::mouseReleaseEvent(QMouseEvent *event){

}

void Scene::defCam()
{
    cam_y=3.5f;
    cam_angle=-65.0f;
    cam_r=5.9f;
}

void Scene::mouseMoveEvent(QMouseEvent *event){
    start.setX(event->x()-start.x());
    start.setY(event->y()-start.y());

    if(mouse_flag){
        this->cam_y+=start.y()/100.0f;
        this->cam_angle+=start.x()/10.0f;
    }else{
        this->angle_x+=start.y()/10.0f;
        this->angle_y+=start.x()/10.0f;
    }


    start=event->pos();
    update();
}

