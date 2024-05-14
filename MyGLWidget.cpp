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
  glEnable (GL_CULL_FACE);

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

  colorLlum = glm::vec3(0.7f, 0.7f, 0.7f);  
  colorLlum2 = glm::vec3(0.5f, 0.5f, 0.5f);  
  ambient = glm::vec3(0.2, 0.2, 0.2);
  posLlum = glm::vec3(marblePos.x, marblePos.y + 3, marblePos.z);
  posLlum2 = glm::vec3(0, 2, 0);

  llum1_on = true;
  llum2_on = true;

  glUniform3fv(colorFocusLoc, 1, &colorLlum[0]);
  glUniform3fv(colorFocusLoc2, 1, &colorLlum2[0]);
  glUniform3fv(llumAbientLoc, 1, &ambient[0]);
  glUniform3fv(posFocusLoc, 1, &posLlum[0]);
  glUniform3fv(posFocusLoc2, 1, &posLlum2[0]);
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

void MyGLWidget::updateSnakeGame()
{
	glm::vec3 newPos = headPos + direction;
	
        if (checkPosition(newPos)) // check boundary limits, check if eats himself
	{
		bodyPos.push_back(headPos);	
		headPos = newPos;
		
		if (direction == glm::vec3(0,0,-1))
			headAngle = 90;
		if (direction == glm::vec3(0,0,1))
			headAngle = -90;
		if (direction == glm::vec3(-1,0,0))
			headAngle = 180;
		if (direction == glm::vec3(1,0,0))
			headAngle = 0;			
	
		if (glm::distance(newPos,marblePos) > 0) // check if doesn't eat marble 
		{
			tailPos = bodyPos.front();
			bodyPos.erase(bodyPos.begin());	
			glm::vec3 tailMov = bodyPos.front() - tailPos;
			tailAngle = signedAngleBetweenVectors(glm::vec3(1,0,0),tailMov);
		}
		else {
			computeRandomMarblePosition();
      posLlum = glm::vec3(marblePos.x, marblePos.y + 3, marblePos.z);
      glUniform3fv(posFocusLoc, 1, &posLlum[0]);
      if (llum1_on) {
        colorLlum = glm::vec3((std::rand()%100) / 100.f, (std::rand()%100) / 100.f , (std::rand()%100) / 100.f);
        glUniform3fv(colorFocusLoc, 1, &colorLlum[0]);
      }
    }
	}
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

void MyGLWidget::creaBuffersModels () {
    // Càrrega dels models
  models[SNAKE_HEAD].load("./models/SnakeHead.obj");
  models[SNAKE_BODY].load("./models/SnakeBody.obj");
  models[SNAKE_TAIL].load("./models/SnakeTail.obj");
  models[MARBLE].load("./models/Marble.obj");
  models[PIPE].load("./models/GreenPipe.obj");

  // Creació de VAOs i VBOs per pintar els models
  glGenVertexArrays(NUM_MODELS, &VAO_models[0]);
  
  float alcadaDesitjada[NUM_MODELS] = {1,1,1,1.5,1}; 
  
  for (int i = 0; i < NUM_MODELS; i++)
  {	
	  // Calculem la capsa contenidora del model
	  calculaCapsaModel (models[i], escalaModels[i], alcadaDesitjada[i], centreBaseModels[i]);
  
	  glBindVertexArray(VAO_models[i]);

	  GLuint VBO[6];
	  glGenBuffers(6, VBO);

	  // geometria
	  glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*models[i].faces().size()*3*3,
		   models[i].VBO_vertices(), GL_STATIC_DRAW);
	  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	  glEnableVertexAttribArray(vertexLoc);

	  // color
	  glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*models[i].faces().size()*3*3,
		   models[i].VBO_normals(), GL_STATIC_DRAW);
	  glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	  glEnableVertexAttribArray(normalLoc);  

    	  glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*models[i].faces().size()*3*3,
		   models[i].VBO_matamb(), GL_STATIC_DRAW);
	  glVertexAttribPointer(matambLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	  glEnableVertexAttribArray(matambLoc);  

    	  glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
	  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*models[i].faces().size()*3*3,
		   models[i].VBO_matdiff(), GL_STATIC_DRAW);
	  glVertexAttribPointer(matdiffLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	  glEnableVertexAttribArray(matdiffLoc);  

    	  glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
	  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*models[i].faces().size()*3*3,
		   models[i].VBO_matspec(), GL_STATIC_DRAW);
	  glVertexAttribPointer(matspecLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	  glEnableVertexAttribArray(matspecLoc);  

    	  glBindBuffer(GL_ARRAY_BUFFER, VBO[5]);
	  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*models[i].faces().size()*3,
		   models[i].VBO_matshin(), GL_STATIC_DRAW);
	  glVertexAttribPointer(matshinLoc, 1, GL_FLOAT, GL_FALSE, 0, 0);
	  glEnableVertexAttribArray(matshinLoc);  
  }
  
  glBindVertexArray (0);
}

void MyGLWidget::iniMaterialTerra ()
{
  // Donem valors al material del terra
  amb = glm::vec3(0.2,0,0.2);
  diff = glm::vec3(0.8,0,0.8);
  spec = glm::vec3(0,0,0);
  shin = 100;
}

void MyGLWidget::creaBuffersTerra ()
{
  // VBO amb la posició dels vèrtexs
  glm::vec3 posterra[4] = {
        glm::vec3(-5.0, 0.0, -5.0),
        glm::vec3(-5.0, 0.0,  5.0),
        glm::vec3( 5.0, 0.0, -5.0),
        glm::vec3( 5.0, 0.0,  5.0)
  }; 


  // VBO amb la normal de cada vèrtex
  glm::vec3 norm1 (0,1,0);
  glm::vec3 norm2 (0,0,1);
  glm::vec3 normterra[4] = {
	norm1, norm1, norm1, norm1};

  iniMaterialTerra();

  // Fem que el material del terra afecti a tots els vèrtexs per igual
  glm::vec3 matambterra[4] = {
	amb, amb, amb, amb};
  glm::vec3 matdiffterra[4] = {
	diff, diff, diff, diff};
  glm::vec3 matspecterra[4] = {
	spec, spec, spec, spec};
  float matshinterra[4] = {
	shin, shin, shin, shin};

// Creació del Vertex Array Object del terra
  glGenVertexArrays(1, &VAO_Terra);
  glBindVertexArray(VAO_Terra);

  GLuint VBO_Terra[6];
  glGenBuffers(6, VBO_Terra);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Terra[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(posterra), posterra, GL_STATIC_DRAW);

  // Activem l'atribut vertexLoc
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  glBindBuffer(GL_ARRAY_BUFFER, VBO_Terra[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(normterra), normterra, GL_STATIC_DRAW);

  // Activem l'atribut normalLoc
  glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(normalLoc);

  // En lloc del color, ara passem tots els paràmetres dels materials
  // Buffer de component ambient
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Terra[2]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(matambterra), matambterra, GL_STATIC_DRAW);

  glVertexAttribPointer(matambLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matambLoc);

  // Buffer de component difusa
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Terra[3]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(matdiffterra), matdiffterra, GL_STATIC_DRAW);

  glVertexAttribPointer(matdiffLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matdiffLoc);

  // Buffer de component especular
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Terra[4]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(matspecterra), matspecterra, GL_STATIC_DRAW);

  glVertexAttribPointer(matspecLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matspecLoc);

  // Buffer de component shininness
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Terra[5]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(matshinterra), matshinterra, GL_STATIC_DRAW);

  glVertexAttribPointer(matshinLoc, 1, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matshinLoc);

  glBindVertexArray(0);
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
      viewTransform();
      paintGL();
      break;
        }  
    case Qt::Key_W: {
      posLlum2 = glm::vec3(posLlum2.x, posLlum2.y, posLlum2.z - 0.5f);
      glUniform3fv(posFocusLoc2, 1, &posLlum2[0]);
      paintGL();
      break;
        }  
    case Qt::Key_S: {
      posLlum2 = glm::vec3(posLlum2.x, posLlum2.y, posLlum2.z + 0.5f);
      glUniform3fv(posFocusLoc2, 1, &posLlum2[0]);
      paintGL();
      break;
        }  
   case Qt::Key_A: { 
      posLlum2 = glm::vec3(posLlum2.x - 0.5f, posLlum2.y, posLlum2.z);
      glUniform3fv(posFocusLoc2, 1, &posLlum2[0]);
      paintGL();
      break;
        }  
   case Qt::Key_D: { 
      posLlum2 = glm::vec3(posLlum2.x + 0.5f, posLlum2.y, posLlum2.z);
      glUniform3fv(posFocusLoc2, 1, &posLlum2[0]);
      paintGL();
      break;
        }  
    case Qt::Key_E: { 
      posLlum2 = glm::vec3(posLlum2.x, posLlum2.y - 0.5f, posLlum2.z);
      glUniform3fv(posFocusLoc2, 1, &posLlum2[0]);
      paintGL();
      break;
        }  
   case Qt::Key_Q: { 
      posLlum2 = glm::vec3(posLlum2.x, posLlum2.y+ 0.5f, posLlum2.z);
      glUniform3fv(posFocusLoc2, 1, &posLlum2[0]);
      paintGL();
      break;
        }  
   case Qt::Key_1: { 
      if (llum1_on) {
        aux1 = glm::vec3(0.f, 0.f, 0.f);
        glUniform3fv(colorFocusLoc, 1, &aux1[0]);
        paintGL();
        llum1_on = false;
        }
      else {
        glUniform3fv(colorFocusLoc, 1, &colorLlum[0]);
        paintGL();
        llum1_on = true;
        }  
      break;
      }
   case Qt::Key_2: { 
      if (llum2_on) {
        aux1 = glm::vec3(0.f, 0.f, 0.f);
        glUniform3fv(colorFocusLoc2, 1, &aux1[0]);
        paintGL();
        llum2_on = false;
        }
      else {
        glUniform3fv(colorFocusLoc2, 1, &colorLlum2[0]);
        paintGL();
        llum2_on = true;
        }  
      break;
      }
    case Qt::Key_T: 
    /*
      else {
        timer.start(200);
        timer_on = true;
      }
      break;
        }  
        */
    default: event->ignore(); break;
  }
  update();
}

void MyGLWidget::cambiarColorLlumR(int r) {
  makeCurrent();
  colorLlum2 = glm::vec3(r / 100.f, colorLlum2.g, colorLlum2.b);
  glUniform3fv(colorFocusLoc2, 1, &colorLlum2[0]);
  paintGL();
  update();
}

void MyGLWidget::cambiarColorLlumG(int g) {
  makeCurrent();
  colorLlum2 = glm::vec3(colorLlum2.r, g / 100.f, colorLlum2.b);
  glUniform3fv(colorFocusLoc2, 1, &colorLlum2[0]);
  paintGL();
  update();
}

void MyGLWidget::cambiarColorLlumB(int b) {
  makeCurrent();
  colorLlum2 = glm::vec3(colorLlum2.r, colorLlum2.g, b / 100.f);
  glUniform3fv(colorFocusLoc2, 1, &colorLlum2[0]);
  paintGL();
  update();
}

//  std::cout << colorLlum.r << ' ' << colorLlum.g << ' ' << colorLlum.b << std::endl;

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

void MyGLWidget::carregaShaders() {
    // Creem els shaders per al fragment shader i el vertex shader
  QOpenGLShader fs (QOpenGLShader::Fragment, this);
  QOpenGLShader vs (QOpenGLShader::Vertex, this);
  // Carreguem el codi dels fitxers i els compilem
  fs.compileSourceFile("shaders/basicShader.frag");
  vs.compileSourceFile("shaders/basicShader.vert");
  // Creem el program
  program = new QOpenGLShaderProgram(this);
  // Li afegim els shaders corresponents
  program->addShader(&fs);
  program->addShader(&vs);
  // Linkem el program
  program->link();
  // Indiquem que aquest és el program que volem usar
  program->bind();

  // Identificador per als  atributs
  vertexLoc = glGetAttribLocation (program->programId(), "vertex");
  normalLoc = glGetAttribLocation (program->programId(), "normal");
  matambLoc = glGetAttribLocation (program->programId(), "matamb");
  matdiffLoc = glGetAttribLocation (program->programId(), "matdiff");
  matshinLoc = glGetAttribLocation (program->programId(), "matshin");
  matspecLoc = glGetAttribLocation (program->programId(), "matspec");

  // Identificadors dels uniform locations
  transLoc = glGetUniformLocation(program->programId(), "TG");
  projLoc  = glGetUniformLocation (program->programId(), "Proj");
  viewLoc  = glGetUniformLocation (program->programId(), "View");
  colorFocusLoc = glGetUniformLocation(program->programId(), "colorFocus");
  colorFocusLoc2 = glGetUniformLocation(program->programId(), "colorFocus2");
  llumAbientLoc  = glGetUniformLocation (program->programId(), "llumAmbient");
  posFocusLoc  = glGetUniformLocation (program->programId(), "posFocus");
  posFocusLoc2  = glGetUniformLocation (program->programId(), "posFocus2");
}





