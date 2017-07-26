//
// The MIT License(MIT)
//
// Copyright(c) 2014 Demonsaw LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <QEvent>

#include "dialog/archive_dialog.h"

namespace eja
{
	archive_dialog::archive_dialog(QWidget* parent /*= 0*/) : QFileDialog(parent)
	{
		setOptions(QFileDialog::ReadOnly); 
		setOption(QFileDialog::DontUseNativeDialog, true);
		setFileMode(QFileDialog::Directory);
		setViewMode(QFileDialog::ViewMode::List);

		QList<QPushButton*> btns = findChildren<QPushButton*>();
		for (int i = 0; i < btns.size(); ++i)
		{
			QString text = btns[i]->text();
			if (text.toLower().contains("open") || text.toLower().contains("choose"))
			{
				m_btnOpen = btns[i];
				break;
			}
		}

		if (!m_btnOpen)
			return;

		m_btnOpen->installEventFilter(this);
		m_btnOpen->disconnect(SIGNAL(clicked()));
		connect(m_btnOpen, SIGNAL(clicked()), this, SLOT(chooseClicked()));

		m_listView = findChild<QListView*>("listView");
		if (m_listView)
			m_listView->setSelectionMode(QAbstractItemView::ExtendedSelection);

		m_treeView = findChild<QTreeView*>();
		if (m_treeView)
			m_treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
	}

	bool archive_dialog::eventFilter(QObject* watched, QEvent* event)
	{
		QPushButton *btn = qobject_cast<QPushButton*>(watched);
		if (btn)
		{
			if (event->type() == QEvent::EnabledChange) 
			{
				if (!btn->isEnabled())
					btn->setEnabled(true);
			}
		}

		return QWidget::eventFilter(watched, event);
	}


	void archive_dialog::chooseClicked()
	{
		const auto index_list = m_listView->selectionModel()->selectedIndexes();

		for (const auto& index : index_list)
		{
			if (index.column() == 0)
			{
				auto file = index.data().toString();
				const auto folder = directory().path();
#if _WIN32
				if (folder == ".")
				{
					if (file.endsWith(":)"))
						file = file.mid(file.length() - 3, 2);

					m_selectedFiles.append(file);
				}
				else
#endif
				{
					// FIX: Detect navigation into a folder without selections
					if (folder.endsWith(file))
					{
						m_selectedFiles.append(folder);
					}
					else
					{
						const auto path = directory().absoluteFilePath(file);
						m_selectedFiles.append(path);
					}					
				}
			}
		}

		QDialog::accept();
	}
}
