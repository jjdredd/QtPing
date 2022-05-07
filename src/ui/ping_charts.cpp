#include <algorithm>

#include <QVector>

#include "ping_charts.hpp"

PingLineChart::PingLineChart(unsigned npoints)
	: PingChartsBase(npoints) {

	m_axisX.setRange(1, m_npoints);
	// axisX->setLabelFormat("%g");
	m_axisX.setTitleText("Sequence Number");
	m_chart.addAxis(&m_axisX, Qt::AlignBottom);

	m_axisY.setRange(0, 100);
	m_axisY.setTitleText("Latency (ms)");
	m_chart.addAxis(&m_axisY, Qt::AlignLeft);

	m_chart.addSeries(&m_series);
	m_series.attachAxis(&m_axisX); // must be done after series is added to chart!!
	m_series.attachAxis(&m_axisY);

}

PingLineChart::~PingLineChart() {}

void PingLineChart::pointsMaxMin(const QVector<QPointF> &points) {

	auto it_xmax = std::max_element(points.begin(), points.end(),
					[] (const QPointF &left, const QPointF &right)
					-> bool {
					    return left.x() < right.x();
				    });

	auto it_ymax = std::max_element(points.begin(), points.end(),
					[] (const QPointF &left, const QPointF &right)
					-> bool {
						return left.y() < right.y();
					});

	m_xmax = std::max((float) it_xmax->x(), (float) m_npoints + 1);
	m_ymax = it_ymax->y();
}

void PingLineChart::ReplacePoints(const QVector<QPointF> &points) {
	pointsMaxMin(points);
	m_axisX.setRange(m_xmax - m_npoints, m_xmax);
	m_axisY.setRange(0, m_ymax + 1); // offset for better picture
	m_series.replace(points);
}
