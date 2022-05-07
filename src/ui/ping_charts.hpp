// charts of ping over time (sequence number)

#pragma once

#include <QVector>
#include <QtCharts>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>


// 
// Base class:
// there could be different chart types, which will be implemented in
// the child classes
// 

class PingChartsBase {

public:
	PingChartsBase(unsigned npoints)
		: m_npoints(npoints) {

		m_chart.legend()->hide();
		m_chart.createDefaultAxes();
		m_chart.setTitle("Latency plot.");
	}

	virtual ~PingChartsBase() {};

	virtual void ReplacePoints(const QVector<QPointF> &) = 0;
	void SetChartView(QChartView *view) { view->setChart(&m_chart); }

protected:
	unsigned m_npoints;
	QChart m_chart;
};


//
// child class implementing line charts
// 

class PingLineChart : public PingChartsBase {

public:
	PingLineChart(unsigned);
	~PingLineChart();

	void ReplacePoints(const QVector<QPointF> &) override;

private:
	void pointsMaxMin(const QVector<QPointF> &);

	QLineSeries m_series;
	QValueAxis m_axisX, m_axisY;
	float m_xmax, m_ymax;
};
