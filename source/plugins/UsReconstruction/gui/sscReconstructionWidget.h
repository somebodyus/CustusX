#ifndef SSCRECONSTRUCTIONWIDGET_H_
#define SSCRECONSTRUCTIONWIDGET_H_

#include <QtGui>
#include "sscReconstructManager.h"
#include "sscDoubleWidgets.h"
#include "sscXmlOptionItem.h"
#include "sscLabeledComboBoxWidget.h"
#include "sscFileSelectWidget.h"
#include "cxBaseWidget.h"

namespace cx
{
class TimedAlgorithmProgressBar;
}

namespace cx
{
/**
 * \file
 * \addtogroup cxPluginUsReconstruction
 * @{
 */

/**
 *  sscReconstructionWidget.h
 *
 *  Created by Ole Vegard Solberg on 5/4/10.
 *
 */
class ReconstructionWidget: public BaseWidget
{
Q_OBJECT
public:
	ReconstructionWidget(QWidget* parent, ReconstructManagerPtr reconstructer);
	ReconstructManagerPtr reconstructer()
	{
		return mReconstructer;
	}

public slots:
	void selectData(QString inputfile);
	void reconstruct();
	void reload();
	void paramsChangedSlot();
private slots:
	void inputDataSelected(QString mhdFileName);
	/** Add the widgets for the current algorithm to a stacked widget.*/
	void repopulateAlgorithmGroup();
	void reconstructStartedSlot();
	void reconstructFinishedSlot();
	void toggleDetailsSlot();

	void reconstructAboutToStartSlot();

private:
	ReconstructManagerPtr mReconstructer;
//	ThreadedTimedReconstructerPtr mThreadedTimedReconstructer;

	FileSelectWidget* mFileSelectWidget;
	QPushButton* mReconstructButton;
	QPushButton* mReloadButton;
	QLineEdit* mExtentLineEdit;
	QLineEdit* mInputSpacingLineEdit;
	SpinBoxGroupWidget* mMaxVolSizeWidget;
	SpinBoxGroupWidget* mSpacingWidget;
	SpinBoxGroupWidget* mDimXWidget;
	SpinBoxGroupWidget* mDimYWidget;
	SpinBoxGroupWidget* mDimZWidget;
	TimedAlgorithmProgressBar* mTimedAlgorithmProgressBar;

	QFrame* mAlgorithmGroup;
	QStackedLayout* mAlgoLayout;
	std::vector<QWidget*> mAlgoWidgets;

	QWidget* mOptionsWidget;
	QWidget* createOptionsWidget();
	QString getCurrentPath();
	void updateComboBox();
	QString defaultWhatsThis() const;
};

/**
 * @}
 */
}//namespace
#endif //SSCRECONSTRUCTIONWIDGET_H_
