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

  public slots:
    void animar();


  private:
  
    int printOglError(const char file[], int line, const char func[]);
    bool ortogonal;
    QTimer timer;
    bool timer_on;
};
