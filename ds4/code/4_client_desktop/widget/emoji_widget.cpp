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

#include <QButtonGroup>
#include <QFont>
#include <QHBoxLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QVBoxLayout>

#include "resource/emoji.h"
#include "resource/resource.h"
#include "widget/emoji_widget.h"

namespace eja
{
	// Static
	const size_t emoji_widget::s_num_smilies = (sizeof(s_smilies) / sizeof(char*));
	const size_t emoji_widget::s_num_animals = (sizeof(s_animals) / sizeof(char*));
	const size_t emoji_widget::s_num_food = (sizeof(s_food) / sizeof(char*));
	const size_t emoji_widget::s_num_sports = (sizeof(s_sports) / sizeof(char*));
	const size_t emoji_widget::s_num_places = (sizeof(s_places) / sizeof(char*));
	const size_t emoji_widget::s_num_objects = (sizeof(s_objects) / sizeof(char*));
	const size_t emoji_widget::s_num_symbols = (sizeof(s_symbols) / sizeof(char*));

	// Constructor
	emoji_widget::emoji_widget(QWidget* parent /*= nullptr*/) : QWidget(parent)
	{
		m_font = QFont(font_family::emoji);

		// HACK: Compute the emoji size accurately (eventually)
		const auto ratio = resource::get_ratio();
#if _WSTRING
		m_font.setPixelSize(resource::get_small_icon_size());
#else
		m_font.setPixelSize(resource::get_icon_size());
#endif
		// Default Cell Size
		m_cell_size = QSize(30 * ratio, 30 * ratio);

		// List
		m_list = new QListWidget(this);
		m_list->setViewMode(QListView::IconMode);
		m_list->setContentsMargins(0, 0, 0, 0);
		m_list->setResizeMode(QListView::Adjust);
		set_emojis(s_smilies, s_num_smilies);

		// Button
		m_button_group = new QButtonGroup(this);
		auto smilies_button = make_button(s_smilies[11], "Faces & People", "emoji_category_first");
		smilies_button->setChecked(true);

		auto animals_button = make_button(s_animals[8], "Animals & Nature");
		auto food_button = make_button(s_food[0], "Food & Drink");
		auto sports_button = make_button(s_sports[2], "Activity & Sports");
		auto places_button = make_button(s_places[45], "Travel & Places");
		auto objects_button = make_button(s_objects[62], "Objects & Things");
		auto symbols_button = make_button(s_symbols[0], "Symbols & Shapes");

		// Layout
		auto hlayout = new QHBoxLayout;
		hlayout->setContentsMargins(0, 0, 0, 0);
		hlayout->setSpacing(0);

		hlayout->addWidget(smilies_button, Qt::AlignCenter);
		hlayout->addWidget(animals_button, Qt::AlignCenter);
		hlayout->addWidget(food_button, Qt::AlignCenter);
		hlayout->addWidget(sports_button, Qt::AlignCenter);
		hlayout->addWidget(places_button, Qt::AlignCenter);
		hlayout->addWidget(objects_button, Qt::AlignCenter);
		hlayout->addWidget(symbols_button, Qt::AlignCenter);

		QWidget* container = new QWidget(this);
		container->setLayout(hlayout);
		container->setObjectName("emoji_categories");

		const auto vlayout = resource::get_vbox_layout(this);
		vlayout->setContentsMargins(0, 0, 0, 0);
		vlayout->setSpacing(0);
		vlayout->addWidget(container);
		vlayout->addWidget(m_list);

		setLayout(vlayout);
		hide();

		// Signal
		connect(m_list, &QListWidget::itemClicked, this, &emoji_widget::emoji_clicked);
		connect(smilies_button, &QPushButton::clicked, this, [&] { set_emojis(s_smilies, s_num_smilies); });
		connect(animals_button, &QPushButton::clicked, this, [&] { set_emojis(s_animals, s_num_animals); });
		connect(food_button, &QPushButton::clicked, this, [&] { set_emojis(s_food, s_num_food); });
		connect(sports_button, &QPushButton::clicked, this, [&] { set_emojis(s_sports, s_num_sports); });
		connect(places_button, &QPushButton::clicked, this, [&] { set_emojis(s_places, s_num_places); });
		connect(objects_button, &QPushButton::clicked, this, [&] { set_emojis(s_objects, s_num_objects); });
		connect(symbols_button, &QPushButton::clicked, this, [&] { set_emojis(s_symbols, s_num_symbols); });
	}

	QPushButton* emoji_widget::make_button(const char* emoji, const char* tooltip, const char* name /*= "emoji_category"*/)
	{
		auto button = new QPushButton(this);
		button->setText(emoji);
		button->setToolTip(tooltip);
		button->setFont(m_font);
		button->setObjectName(name);
		button->setCheckable(true);

		m_button_group->addButton(button);
		
		return button;
	}
	
	void emoji_widget::set_emojis(const char* emojis[], const size_t length)
	{
		m_list->clear();

		for (auto i = 0; i < length; ++i)
		{
			auto item = new QListWidgetItem(QString::fromUtf8(emojis[i]));
			item->setFont(m_font);
			item->setFlags(Qt::ItemIsSelectable);
			item->setTextAlignment(Qt::AlignCenter);
			item->setSizeHint(m_cell_size);
			m_list->addItem(item);
		}
	}

	void emoji_widget::focusOutEvent(QFocusEvent* event)
	{
		if (!m_activator_button->hasFocus() && !underMouse())
			toggle();
		else if (!m_activator_button->hasFocus())
			setFocus();
	}

	void emoji_widget::toggle()
	{
		if (isVisible())
		{
			hide();
			m_activator_button->setChecked(false);
			emit closing();
		}
		else
		{
			show();
			raise();
			setFocus(Qt::FocusReason::PopupFocusReason);
			m_activator_button->setChecked(true);
		}
	}
}
