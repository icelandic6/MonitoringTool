#include "MonitoringToolWidget.h"
#include "TCPServerMonitor.h"

#include <QPushButton>
#include <QHBoxLayout>
#include <QHostAddress>

class ui::MonitoringToolWidgetPrivate : public QObject
{
    Q_DECLARE_PUBLIC(MonitoringToolWidget)
    Q_DISABLE_COPY(MonitoringToolWidgetPrivate)
    MonitoringToolWidget *q_ptr = nullptr;

    QPushButton *_checkServerButton = nullptr;

    ServerMonitor *_serverMonitor = nullptr;

public:
    explicit MonitoringToolWidgetPrivate(MonitoringToolWidget *q)
        : q_ptr(q)
    {
    }

    ~MonitoringToolWidgetPrivate()
    {
    }

private:
    void createCheckServerButton()
    {
        Q_Q(MonitoringToolWidget);

        _checkServerButton = new QPushButton("Check server", q);
        _checkServerButton->setFixedSize(200, 40);
        _checkServerButton->setAutoFillBackground(true);

        connect(_checkServerButton, &QPushButton::pressed, this, [this]()
        {
            QPalette pal = _checkServerButton->palette();
            pal.setColor(QPalette::Button, QColor(100, 100, 100));
            pal.setColor(QPalette::Base, QColor(100, 100, 100));
            _checkServerButton->setPalette(pal);
            _checkServerButton->update();
        });
    }

    void createServerMonitor()
    {
        Q_Q(MonitoringToolWidget);

//         QString addr("google.com");
        QString addr("http://example.com");

        _serverMonitor = new TCPServerMonitor(
            "TCP Host",
            addr,
            80,
            q);

        connect(_serverMonitor, &ServerMonitor::succeeded, this, [this]()
        {
            QPalette pal = _checkServerButton->palette();
            pal.setColor(QPalette::Button, QColor(100, 200, 100));
            pal.setColor(QPalette::Window, QColor(100, 200, 100));
            _checkServerButton->setPalette(pal);
        });
        connect(_serverMonitor, &ServerMonitor::failed, this, [this]()
        {
            QPalette pal = _checkServerButton->palette();
            pal.setColor(QPalette::Button, QColor(200, 100, 100));
            pal.setColor(QPalette::Window, QColor(200, 100, 100));
            _checkServerButton->setPalette(pal);
        });
    }

    void createLayouts()
    {
        Q_Q(MonitoringToolWidget);

        auto mainLayout = new QHBoxLayout();
        mainLayout->setContentsMargins(QMargins(0, 0, 0, 0));
        mainLayout->setSpacing(0);
        mainLayout->addWidget(_checkServerButton, 0, Qt::AlignCenter);

        q->setLayout(mainLayout);
    }

public:
	void checkServer()
	{
		_serverMonitor->checkServer();
	}
};
ui::MonitoringToolWidget::MonitoringToolWidget(QWidget *parent)
    : QWidget(parent)
	, d_ptr(new MonitoringToolWidgetPrivate(this))
{
	Q_D(MonitoringToolWidget);

	setMinimumSize(200, 100);

	d->createServerMonitor();

	d->createCheckServerButton();
	d->createLayouts();

	connect(d->_checkServerButton, &QPushButton::clicked, d, &MonitoringToolWidgetPrivate::checkServer);
}

ui::MonitoringToolWidget::~MonitoringToolWidget()
{

}
