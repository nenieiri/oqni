#include "../Includes/threaddrawer.hpp"

ThreadDrawer::ThreadDrawer(QObject *parent)
    : QThread{parent}
{
}

ThreadDrawer::~ThreadDrawer()
{
}
