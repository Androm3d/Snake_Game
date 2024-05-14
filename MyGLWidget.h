#include "LL2GLWidget.h"

#include <vector>

#include <QTimer>

class MyGLWidget : public LL2GLWidget {
  Q_OBJECT

  public:
    MyGLWidget(QWidget *parent);
    ~MyGLWidget();

  protected:
  
    virtual void keyPressEvent (QKeyEvent *event);

    // funció per comprovar si una nova posició de la serp és vàlida
    virtual bool checkPosition(glm::vec3 pos);

    void initializeGL ( );
    virtual void iniCamera ();  
    virtual void iniEscena ();  
    virtual void viewTransform ();
    virtual void viewTransformOrto ();
    virtual void paintGL();
    virtual void projectTransform();

    virtual void mouseMoveEvent (QMouseEvent *event);

    void TerraTransform();
    virtual void PipeTransform(glm::vec3 pos);    
    virtual void SnakeHeadTransform();
    virtual void SnakeBodyTransform(glm::vec3 pos);
    virtual void SnakeTailTransform();
    virtual void MarbleTransform();

        // creaBuffersModels - Carreguem els fitxers obj i fem la inicialització dels diferents VAOS i VBOs
    virtual void creaBuffersModels ();
    // creaBuffersTerra - Inicialitzem el VAO i els VBO d'un model fet a mà que representa un terra
    virtual void creaBuffersTerra ();

    virtual void carregaShaders ();
    virtual void iniMaterialTerra();

    virtual void updateSnakeGame();

  public slots:
    void animar();

    void cambiarColorLlumR(int r);
    void cambiarColorLlumG(int g);
    void cambiarColorLlumB(int b);


  private:
  
    int printOglError(const char file[], int line, const char func[]);
    bool ortogonal;
    QTimer timer;
    bool timer_on, llum1_on, llum2_on;
    GLuint vertexLoc, normalLoc, matambLoc, matdiffLoc, matspecLoc, matshinLoc,
            colorFocusLoc, colorFocusLoc2, llumAbientLoc, posFocusLoc, posFocusLoc2;
    glm::vec3 amb, diff, spec, colorLlum, colorLlum2, ambient, posLlum, posLlum2, aux1, aux2;
    float shin;

};
