#include <QApplication>

#include <RRDFile.hpp>
#include <RRDWidget.hpp>

#include "test_helpers.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    RRDFile rrd(helpers::getDataPath() + "/sample.rrd");
    RRDWidget widget;

    widget.addSource(rrd);
    widget.show();
    return app.exec();
}

