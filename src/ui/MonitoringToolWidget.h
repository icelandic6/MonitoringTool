#pragma once

#include <QtWidgets/QWidget>

namespace ui
{
	class MonitoringToolWidgetPrivate;
	class MonitoringToolWidget : public QWidget
	{
		Q_OBJECT

	public:
		MonitoringToolWidget(QWidget *parent = Q_NULLPTR);
		~MonitoringToolWidget();

	private:
		QScopedPointer<MonitoringToolWidgetPrivate> d_ptr;
		Q_DISABLE_COPY(MonitoringToolWidget)
		Q_DECLARE_PRIVATE(MonitoringToolWidget)
	};
}