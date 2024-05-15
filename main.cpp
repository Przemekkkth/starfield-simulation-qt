#include <QApplication>
#include <QWidget>
#include <QTimer>
#include <QElapsedTimer>
#include <QPainter>
#include <QObject>
#include <QMouseEvent>
#include <QRandomGenerator>
#include <QMouseEvent>

const int WIDTH  = 800;
const int HEIGHT = 800;

// Define a function to map a value from one range to another
float map(float value, float fromLow, float fromHigh, float toLow, float toHigh) {
    if (value < fromLow) {
        value = fromLow;
    }
    else if(value > toHigh) {
        value = toHigh;
    }
    // Perform linear interpolation to map the value to the output range
    return toLow + (value - fromLow) * ((toHigh - toLow) / (fromHigh - fromLow));
}

QColor returnColor(int value) {
    return QColor(255 - (value * 20), 255 - (value * 20), 255 - (value * 20));
}

struct Drop {
    int m_x, m_z, m_len;
    float m_y, m_yspeed;
    Drop() {
        m_x      = QRandomGenerator::global()->bounded(WIDTH); //width
        m_y      = QRandomGenerator::global()->bounded(450) - 500;
        m_z      = QRandomGenerator::global()->bounded(20);
        m_len    = map(m_z, 0, 20, 10, 20);
        m_yspeed = map(m_z, 0, 20,  1, 20);
    }

    void update(float dt = 1/60) {
        m_y = m_y + m_yspeed;
        float grav = map(m_z, 0, 20, 0, 0.2);
        m_yspeed = m_yspeed + (grav*dt);
        if (m_y > HEIGHT) { //height
            m_y = QRandomGenerator::global()->bounded(100) - 200;
            m_yspeed = map(m_z, 0, 20, 4, 10);
        }
    }

    void draw(QPainter *painter) {
        painter->save();
        float thick = map(m_z, 0, 20, 1, 3);
        int roundedThick = qRound(thick);
        QColor color = returnColor(m_z);
        painter->setPen(QPen(color));
        painter->setBrush(QBrush(color));
        painter->drawRect(m_x, m_y, thick, m_len + roundedThick);
        painter->restore();
    }
};

class Star {
public:
    Star() {
        m_x = QRandomGenerator::global()->bounded(-WIDTH, WIDTH);
        m_y = QRandomGenerator::global()->bounded(-HEIGHT, HEIGHT);
        m_z = QRandomGenerator::global()->bounded(WIDTH);
        m_pz = m_z;
        m_speed = 20.0f;
    }

    void update() {
        m_z = m_z - m_speed;
        if (m_z < 1) {
            m_z = WIDTH;
            m_x = QRandomGenerator::global()->bounded(-WIDTH, WIDTH);
            m_y = QRandomGenerator::global()->bounded(-HEIGHT, HEIGHT);
            m_pz = m_z;
        }
    }

    void draw(QPainter* painter) {
        painter->save();
        painter->setBrush(QBrush(Qt::white));
        painter->setPen(QPen(Qt::NoPen));
        float sx = map(m_x / m_z, -1.0f, 1.0f, -WIDTH, WIDTH);
        float sy = map(m_y / m_z, -1.0f, 1.0f, -HEIGHT, HEIGHT);
        float r  = map(m_z, 0, WIDTH, 4, 0);
        painter->drawEllipse(sx - r / 2, sy - r / 2, r, r);

        float px = map(m_x / m_pz, -1.0f, 1.0f, -WIDTH, WIDTH);
        float py = map(m_y / m_pz, -1.0f, 1.0f, -HEIGHT, HEIGHT);
        m_pz = m_z;
        QPen pen(QBrush(QColor(248,248,186)), 1);
        painter->setPen(pen);
        painter->setBrush(QBrush(QColor(248,248,186)));
        painter->drawLine(QPointF(px, py), QPointF(sx, sy));

        painter->restore();
    }

    void setSpeed(float newSpeed) {
        m_speed = newSpeed;
    }
private:
    float m_x, m_y, m_z, m_pz;
    float m_speed;
};

class Window : public QWidget
{
    Q_OBJECT
public:
    Window(int w, int h, QWidget* parent = nullptr) : QWidget(parent)
    {
        setWindowTitle(QStringLiteral("Starfield Simulation Qt/C++"));
        setMouseTracking(true);
        resize(w, h);
        for(int i = 0; i < 400; ++i) {
            m_stars[i] = new Star();
        }

        QObject::connect(&m_timer, &QTimer::timeout, this, &Window::animationLoop);
        m_timer.start(1000.f/60.f); //60 times per second
        m_elapsedTimer.start();
    }
    virtual ~Window(){
        delete[] *m_stars;
    }

private:
    QTimer m_timer;
    QElapsedTimer m_elapsedTimer;
    Star* m_stars[400];

private slots:
    void animationLoop()
    {
        update();
        for(int i = 0; i < 400; ++i) {
            m_stars[i]->update();
        }
    }
private:
    void paintEvent(QPaintEvent *event)
    {
        Q_UNUSED(event);
        QPainter qp(this);
        qp.setBrush(QBrush(QColor(0, 0, 0)));
        qp.drawRect(0,0,size().width(), size().height());
        qp.translate(WIDTH / 2, HEIGHT / 2);
        for(int i = 0; i < 400; ++i) {
            m_stars[i]->draw(&qp);
        }
    }

    // QWidget interface
protected:
    virtual void mouseMoveEvent(QMouseEvent *event) override {
        for(int i = 0; i < 400; ++i) {
            m_stars[i]->setSpeed(event->position().x() / 20);
        }
    }
};

#include "main.moc"

int main(int argc, char** argv)
{
    QApplication a(argc, argv);
    Window *w = new Window(WIDTH, HEIGHT);
    w->show();
    a.exec();
}


