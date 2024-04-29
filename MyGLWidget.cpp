#include "MyGLWidget.h"
#include <iostream>
#include <stdio.h>

#define printOpenGLError() printOglError(__FILE__, __LINE__)
#define CHECK() printOglError(__FILE__, __LINE__,__FUNCTION__)
#define DEBUG() std::cout << __FILE__ << " " << __LINE__ << " " << __FUNCTION__ << std::endl;

MyGLWidget::MyGLWidget(QWidget *parent=0) : LL2GLWidget(parent) 
{
}

void MyGLWidget::initializeGL ()
{
  // Cal inicialitzar l'ús de les funcions d'OpenGL
  ortogonal = false;
  timer_on = false;
  initializeOpenGLFunctions();  
  glEnable(GL_DEPTH_TEST);

  connect (&timer, SIGNAL (timeout()), this, SLOT (animar ()));

  glClearColor(0.5, 0.7, 1.0, 1.0); // defineix color de fons (d'esborrat)
  carregaShaders();
  creaBuffersModels();
  creaBuffersTerra();
  TerraTransform();
  iniEscena();
  iniCamera();
  paintGL();
}

void MyGLWidget::iniEscena()
{
  radiEscena = sqrt(80);
  centreEscena = glm::vec3(0,0,0);

  factorAngleX = 0;
  factorAngleY = 0;

  headPos = glm::vec3(0,0,0);
  headAngle = 0;
  bodyPos.clear();	    
  bodyPos.push_back(glm::vec3(-1,0,0));    	
  tailPos = glm::vec3(-2,0,0);
  tailAngle = 0;

  marblePos = glm::vec3(10, 0, 0);

  direction = glm::vec3(1,0,0);
}

void MyGLWidget::paintGL ()
{
  // descomentar per canviar paràmetres
  // glViewport (0, 0, ample, alt);

  // Esborrem el frame-buffer
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // SnakeHead
  glBindVertexArray (VAO_models[SNAKE_HEAD]);
  SnakeHeadTransform();
  glDrawArrays(GL_TRIANGLES, 0, models[SNAKE_HEAD].faces().size()*3);
  
  // SnakeBody
  glBindVertexArray (VAO_models[SNAKE_BODY]);
  for (unsigned int i = 0; i < bodyPos.size(); i++)
  {  
	  SnakeBodyTransform(bodyPos[i]);
	  glDrawArrays(GL_TRIANGLES, 0, models[SNAKE_BODY].faces().size()*3);
  }

  // SnakeTail
  glBindVertexArray (VAO_models[SNAKE_TAIL]);
  SnakeTailTransform();
  glDrawArrays(GL_TRIANGLES, 0, models[SNAKE_TAIL].faces().size()*3);
  
  // Marble
  glBindVertexArray (VAO_models[MARBLE]);
  MarbleTransform();
  glDrawArrays(GL_TRIANGLES, 0, models[MARBLE].faces().size()*3);

  // Pipe
  glBindVertexArray (VAO_models[PIPE]);
  for (int i = 0; i < 30; ++i) {
    PipeTransform(glm::vec3(-14.5f, 0.f, -14.5f+i));
    glDrawArrays(GL_TRIANGLES, 0, models[PIPE].faces().size()*3);
    PipeTransform(glm::vec3(14.5f, 0.f, -14.5f+i));
    glDrawArrays(GL_TRIANGLES, 0, models[PIPE].faces().size()*3);
    PipeTransform(glm::vec3(-14.5f+i, 0.f, -14.5f));
    glDrawArrays(GL_TRIANGLES, 0, models[PIPE].faces().size()*3);
    PipeTransform(glm::vec3(-14.5f+i, 0.f, 14.5f));
    glDrawArrays(GL_TRIANGLES, 0, models[PIPE].faces().size()*3);
  }
  
  // Terra
  glBindVertexArray (VAO_Terra);
  TerraTransform();
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  glBindVertexArray (0);
}

// Transforms

void MyGLWidget::TerraTransform ()
{
  glm::mat4 TG(1.0f);
  TG = glm::translate(TG, glm::vec3(0, 0, 0));
  TG = glm::scale(TG, glm::vec3(3, 1, 3));
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::PipeTransform (glm::vec3 pos)
{
  glm::mat4 TG(1.0f);
  TG = glm::translate(TG, pos);
  TG = glm::scale(TG, glm::vec3(escalaModels[PIPE]));
  TG = glm::scale(TG, glm::vec3(1, 3, 1));
  TG = glm::translate(TG, -centreBaseModels[PIPE]);
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::MarbleTransform ()
{
  glm::mat4 TG(1.0f);
  TG = glm::translate(TG, marblePos);
  TG = glm::scale(TG, glm::vec3(escalaModels[MARBLE]));
  TG = glm::scale(TG, glm::vec3(0.5f, 0.5f, 0.5f));
  TG = glm::translate(TG, -centreBaseModels[MARBLE]);
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::SnakeHeadTransform ()
{
  glm::mat4 TG(1.0f);  
  TG = glm::translate(TG, headPos);
  TG = glm::scale(TG, glm::vec3(escalaModels[SNAKE_HEAD]));
  TG = glm::rotate(TG, (headAngle - 90)/57.2958f, glm::vec3(0,1,0));
  TG = glm::translate(TG, -centreBaseModels[SNAKE_HEAD]);
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::SnakeBodyTransform (glm::vec3 pos)
{
  glm::mat4 TG(1.0f);
  TG = glm::translate(TG, pos);
  TG = glm::scale(TG, glm::vec3(escalaModels[SNAKE_BODY]));
  TG = glm::translate(TG, -centreBaseModels[SNAKE_BODY]);
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::SnakeTailTransform ()
{
  glm::mat4 TG(1.0f);
  TG = glm::translate(TG, tailPos);
  TG = glm::scale(TG, glm::vec3(escalaModels[SNAKE_TAIL]));
  TG = glm::rotate(TG, (tailAngle)/57.2958f, glm::vec3(0,1,0));
  TG = glm::translate(TG, -centreBaseModels[SNAKE_TAIL]);
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

int MyGLWidget::printOglError(const char file[], int line, const char func[]) 
{
    GLenum glErr;
    int    retCode = 0;

    glErr = glGetError();
    const char * error = 0;
    switch (glErr)
    {
        case 0x0500:
            error = "GL_INVALID_ENUM";
            break;
        case 0x501:
            error = "GL_INVALID_VALUE";
            break;
        case 0x502: 
            error = "GL_INVALID_OPERATION";
            break;
        case 0x503:
            error = "GL_STACK_OVERFLOW";
            break;
        case 0x504:
            error = "GL_STACK_UNDERFLOW";
            break;
        case 0x505:
            error = "GL_OUT_OF_MEMORY";
            break;
        default:
            error = "unknown error!";
    }
    if (glErr != GL_NO_ERROR)
    {
        printf("glError in file %s @ line %d: %s function: %s\n",
                             file, line, error, func);
        retCode = 1;
    }
    return retCode;
}

MyGLWidget::~MyGLWidget()
{
}



bool MyGLWidget::checkPosition(glm::vec3 pos)
{
	bool valid = true;
  valid = pos.x < 14 and pos.x > -14 and pos.z < 14 and pos.z > -14;
  valid &= not (pos == tailPos);
  for(unsigned int i = 0; i < bodyPos.size() and valid; ++i) {
    valid = not (pos == bodyPos[i]);
  }
	return valid; 
}

void MyGLWidget::mouseMoveEvent(QMouseEvent *e)
{
  if (not ortogonal) {
    makeCurrent();
    factorAngleX += ((e -> x() - xClick) * M_PI)/ample;
    factorAngleY += ((e -> y() - yClick) * M_PI)/ample;

    xClick = e->x();
    yClick = e->y();

    update ();
    viewTransform();
  }
}

void MyGLWidget::keyPressEvent(QKeyEvent* event) 
{
  makeCurrent();
  switch (event->key()) {
    case Qt::Key_Up: { 
      direction = glm::vec3(0, 0, -1);
      if (not timer_on) updateSnakeGame();
      break;
    }  DEBUG();

    case Qt::Key_Down: { 
      direction = glm::vec3(0, 0, 1);
      if (not timer_on) updateSnakeGame();
      break;
        }
    case Qt::Key_Left: { 
      direction = glm::vec3(-1, 0, 0);
      if (not timer_on) updateSnakeGame();
      break;
        }  
    case Qt::Key_Right: { 
      direction = glm::vec3(1, 0, 0);
      if (not timer_on) updateSnakeGame();
      break;
        }
    case Qt::Key_C: { 
      ortogonal = !ortogonal;
      ortogonal ? viewTransformOrto() : viewTransform();
      projectTransform();
      break;
        }           
    case Qt::Key_R: { 
      iniEscena();
      break;
        }  
    case Qt::Key_T: {
      if(timer_on) {
        timer.stop();
        timer_on = false;
      } 
      else {
        timer.start(200);
        timer_on = true;
      }
      break;
        }  
    default: event->ignore(); break;
  }
  update();
}

void MyGLWidget::animar() {
  paintGL();
  updateSnakeGame();
  DEBUG();
}

void MyGLWidget::iniCamera(){

  obs = glm::vec3(0, 30, -30);
  vrp = glm::vec3(0, -1, -2);
  up = glm::vec3(0, 2, 1);
  fov = 1.0472f;
  znear =  1;
  zfar  = 100;

  viewTransform();
}

void MyGLWidget::viewTransform ()
{
  // Matriu de posició i orientació de l'observador
  glm::mat4 View(1.0f);
  //View = glm::lookAt (obs, vrp, up);
  View = glm::translate(View, glm::vec3(0, 0, -2 * 21.4242852856));
  View = glm::rotate (View, 0.785398f, glm::vec3(1, 0, 0));
  View = glm::rotate (View, factorAngleY, glm::vec3(1, 0, 0));
  View = glm::rotate (View, factorAngleX, glm::vec3(0, 1, 0));
  
  glUniformMatrix4fv (viewLoc, 1, GL_FALSE, &View[0][0]);
}

void MyGLWidget::projectTransform ()
{
  glm::mat4 Proj(1.0f);
  if (!ortogonal) Proj = glm::perspective (fov, ra, znear, zfar);
  else{
    float ra_guay = float(ample)/float(alt);
    if (ra_guay >= 1) Proj = glm::ortho (-21.2132034356f * ra_guay, 21.2132034356f * ra_guay, -21.2132034356f, 21.2132034356f, 0.f, 100.f);
    else Proj = glm::ortho  (-21.2132034356f, 21.2132034356f, -21.2132034356f/ra_guay, 21.2132034356f/ra_guay, 0.f, 100.f);
  }
  glUniformMatrix4fv (projLoc, 1, GL_FALSE, &Proj[0][0]);
}

void MyGLWidget::viewTransformOrto ()
{
  // Matriu de posició i orientació de l'observador
  glm::mat4 View(1.0f);
  //View = glm::lookAt (obs, vrp, up);
  View = glm::translate(View, glm::vec3(0, 0, -50));
  View = glm::rotate (View, 1.5708f, glm::vec3(1, 0, 0));
  
  glUniformMatrix4fv (viewLoc, 1, GL_FALSE, &View[0][0]);
}









