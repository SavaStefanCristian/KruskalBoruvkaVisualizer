#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPainter>

#include <QWidget>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QButtonGroup>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    isFirstNodeSelected = false;
    isDragging = false;
    srand(time(NULL));
    m_cost = 0;
    showingSort = false;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mousePressEvent(QMouseEvent *m)
{
    if(!isFirstNodeSelected) return;
    if(m->button() == Qt::LeftButton)
    {
        std::vector<Node*> nodes = m_graph.GetNodes();
        for(Node* n : nodes) {
            if((n->GetPos().x() - m->pos().x())*(n->GetPos().x() - m->pos().x())+
                    (n->GetPos().y() - m->pos().y())*(n->GetPos().y() - m->pos().y())
                < m_radius*m_radius) {
                if(isFirstNodeSelected)
                {
                    if(m_selectedNode==n) {
                        isDragging = true;
                        m_selectedNodePreviusCoords = m_selectedNode->GetPos();
                    }
                }
                update();
                break;
            }
        }
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *m) {
    if(isDragging) {
        m_selectedNode->SetPos(m->pos());
        update();
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *m)
{
    if (m->button() == Qt::RightButton)
    {
        std::vector<Node*> nodes = m_graph.GetNodes();
        for(Node* n : nodes)
        {
            if((n->GetPos().x() - m->pos().x())*(n->GetPos().x() - m->pos().x())+
                    (n->GetPos().y() - m->pos().y())*(n->GetPos().y() - m->pos().y())
                        < 2*2*m_radius*m_radius) {
                return;
            }
        }
        m_graph.AddNode(m->pos());
        showingSort = false;
        update();
    }
    else if(m->button() == Qt::LeftButton)
    {
        std::vector<Node*> nodes;
        if(isDragging) {
            nodes = m_graph.GetNodes();
            for(Node* n : nodes) {
                if(n==m_selectedNode) continue;
                if((n->GetPos().x() - m_selectedNode->GetPos().x())*(n->GetPos().x() - m_selectedNode->GetPos().x())+
                    (n->GetPos().y() - m_selectedNode->GetPos().y())*(n->GetPos().y() - m_selectedNode->GetPos().y())
                        < 2*2*m_radius*m_radius) {
                    m_selectedNode->SetPos(m_selectedNodePreviusCoords);
                    break;
                }
            }
            isDragging =false;
            isFirstNodeSelected = false;
            m_selectedNode = nullptr;
            update();
            return;
        }
        nodes = m_graph.GetNodes();
        std::unordered_map<Node*, std::unordered_map<Node*,int>> edges = m_graph.GetEdges();
        for(Node* n : nodes) {
            if((n->GetPos().x() - m->pos().x())*(n->GetPos().x() - m->pos().x())+
                    (n->GetPos().y() - m->pos().y())*(n->GetPos().y() - m->pos().y())
                        < m_radius*m_radius) {
                if(!isFirstNodeSelected) {

                    isFirstNodeSelected = true;
                    m_selectedNode =n;
                }
                else
                {
                    if(m_selectedNode!=n) {
                        if(edges.find(m_selectedNode)!=edges.end() && edges[m_selectedNode].find(n)!=edges[m_selectedNode].end())
                            m_graph.RemoveEdge(m_selectedNode,n);
                            else
                            m_graph.AddEdge(m_selectedNode,n, m_cost);
                        showingSort = false;
                    }
                    isFirstNodeSelected = false;
                    m_selectedNode = nullptr;
                }
                update();
                break;
            }
        }
    }
}

void MainWindow::paintEvent(QPaintEvent *ev)
{
    QPainter p{this};
    std::vector<Node*> nodes = m_graph.GetNodes();
    for(auto& n : nodes)
    {
        QRect r{n->GetPos().x()-m_radius,n->GetPos().y()-m_radius, m_radius*2, m_radius*2};
        if(m_selectedNode == n) {
            p.setPen(QPen(Qt::green, m_radius/10));
        }
        p.drawEllipse(r);
        p.setBrush(QBrush(Qt::NoBrush));
        if(m_selectedNode != nullptr && m_selectedNode == n) {
            p.setPen(Qt::white);
        }
        QString str = QString::number(n->GetValue()+1);
        p.drawText(r, Qt::AlignCenter, str);
    }
    std::unordered_map<Node*,std::unordered_map<Node*,int>> edges = m_graph.GetEdges();
    for(auto& [node,connections] : edges)
    {
        int x1 = node->GetPos().x(), y1 = node->GetPos().y();
        for(auto [currCon,cost] : connections) {
            if(showingSort) {
                bool isPartOfSort = m_sortEdges.find(node)!=m_sortEdges.end() && m_sortEdges[node].find(currCon)!= m_sortEdges[node].end() && m_sortEdges[node][currCon] == cost;
                if(isPartOfSort) {
                    p.setPen(QPen(Qt::green, m_radius/20));
                }
                else {
                    p.setPen(Qt::gray);
                }
            }
            int x2 = currCon->GetPos().x(), y2 = currCon->GetPos().y();
            float x1final, y1final, x2final, y2final;
            std::pair<int,int> results1 = calculateIntersection(x1,y1,x2,y2,m_radius);
            x1final = results1.first;
            y1final = results1.second;
            std::pair<int,int> results2 = calculateIntersection(x2,y2,x1,y1,m_radius);
            x2final = results2.first;
            y2final = results2.second;
            p.drawLine(x1final,y1final,x2final,y2final);
            float xCost = (x1+x2)/2, yCost = (y1+y2)/2;
            QString costString = QString::number(cost);
            QRectF c{xCost-costString.length()*5,yCost - 10.0f, (float)costString.length()*10, 20.0f};
            p.setBrush(QBrush(Qt::white));
            p.drawRect(c);
            p.setBrush(QBrush(Qt::NoBrush));
            p.setPen(Qt::black);
            p.drawText(c, Qt::AlignCenter, costString);
            p.setPen(Qt::white);
        }
    }
}


std::pair<int, int> MainWindow::calculateIntersection(int x1, int y1, int x2, int y2, int radius)
{
    float x1final, y1final;
    if (x1 == x2) {
        float yv1 = y1 + radius;
        float yv2 = y1 - radius;

        if (std::abs(y2 - yv1) < std::abs(y2 - yv2)) {
            y1final = yv1;
        } else {
            y1final = yv2;
        }
        x1final = x1;
    }
    else {
        float m = float(y2 - y1) / float(x2 - x1);
        float b = y1 - m * x1;

        float A = 1 + m * m;
        float B = 2 * (m * (b - y1) - x1);
        float C = (x1 * x1) + (b - y1) * (b - y1) - radius * radius;

        float delta = B * B - 4 * A * C;

        if (delta < 0) {
            float yv1 = y1 + radius;
            float yv2 = y1 - radius;

            if (std::abs(y2 - yv1) < std::abs(y2 - yv2)) {
                y1final = yv1;
            } else {
                y1final = yv2;
            }
            x1final = x1;
            return std::pair<int, int>(x1final, y1final);
        }

        float sqrtDelta = std::sqrt(delta);
        float xv1 = (-B + sqrtDelta) / (2 * A);
        float xv2 = (-B - sqrtDelta) / (2 * A);

        float yv1 = m * xv1 + b;
        float yv2 = m * xv2 + b;

        float dist1 = std::pow(x2 - xv1, 2) + std::pow(y2 - yv1, 2);
        float dist2 = std::pow(x2 - xv2, 2) + std::pow(y2 - yv2, 2);

        if (dist1 < dist2) {
            x1final = xv1;
            y1final = yv1;
        } else {
            x1final = xv2;
            y1final = yv2;
        }
    }

    return std::pair<int,int>(x1final,y1final);
}


void MainWindow::on_costBox_valueChanged(int arg1)
{
    m_cost = arg1;
}


void MainWindow::on_BoruvkaButton_released()
{
    showingSort = true;
    m_sortEdges.clear();
    m_sortEdges = m_graph.Boruvka();
    update();
}


void MainWindow::on_KruskalButton_released()
{
    showingSort = true;
    m_sortEdges.clear();
    m_sortEdges = m_graph.Kruskal();
    update();
}


void MainWindow::on_ResetButton_released()
{
    showingSort = false;
    m_sortEdges.clear();
    update();
}

