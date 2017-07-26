
import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

Item
{
	// Variables
	readonly property bool m_first: index == (Message.size - 1);

	id: id_item;
	height: id_spacing.height + id_text.height;
	width: parent.width;

	//
	// HACK: Optimization for rendering
	//
	Item
	{
		id: m_self;
		enabled: role_self;
		visible: false;
	}

	Item
	{
		id: m_message;
		state: role_message;
		visible: false;
	}

	Item
	{
		id: m_time;
		state: role_time;
		visible: false;
	}

	Rectangle
	{
		id: m_color;
		color: role_color;
		visible: false;
	}

	// Functions
	function get_row()
	{
		return index;
	}

	function get_message()
	{
		return m_message.state;
	}

	// Spacing
	Rectangle
	{
		id: id_spacing

		anchors
		{
			left: parent.left;
			top: parent.top;
		}
		color: "transparent";
		height: padding_size_2 - 1;
		width: parent.width;
	}

	// Color
	Rectangle
	{
		id: id_color_left;
		
		anchors
		{
			left: parent.left;
			top: id_spacing.bottom;
		}
		color: m_color.color;
		height: id_name.height + id_message.height;
		width: color_size;
		visible: !m_self.enabled;
	}
	
	Rectangle
	{
		id: id_color_right;
		
		anchors
		{
			right: parent.right;
			top: id_spacing.bottom;
		}
		color: m_color.color;
		height: id_name.height + id_message.height;
		width: color_size;
		visible: m_self.enabled;
	}

	// Item
	Item
	{
		id: id_text;

		anchors
		{
			bottomMargin: padding_size;
			left: m_self.enabled ? parent.left : id_color_left.right;
			leftMargin: m_self.enabled ? padding_size : padding_size_2;
			right: m_self.enabled ? id_color_right.left : id_item.right;
			rightMargin: m_self.enabled ? padding_size_2 : padding_size;
			top: id_spacing.bottom;
			topMargin: -padding_size;
		}
		height: id_name.height + id_message.height;

		// Name
		Row
		{
			id: id_row;
			width: id_text.width;

			Text
			{
				id: id_time_left;				
				bottomPadding: padding_size_2;
				color: medium_grey;
				font.bold: true;
				font.pixelSize: font_size * 0.90;
				height: font_size + padding_size_3;
				horizontalAlignment: Text.AlignLeft;
				rightPadding: padding_size;
				text: m_time.state;
				topPadding: padding_size_3;
				verticalAlignment: Text.AlignVCenter;
				visible: false;
				width: parent.width * 0.25;
			}

			Text
			{
				id: id_name;
				bottomPadding: padding_size_2;
				color: m_color.color;
				font.bold: true;
				font.pixelSize: font_size;
				height: font_size + padding_size_3;
				horizontalAlignment: m_self.enabled ? Text.AlignRight : Text.AlignLeft;
				rightPadding: padding_size;
				text: role_name;
				textFormat: Text.PlainText;
				topPadding: padding_size_3;
				verticalAlignment: Text.AlignVCenter;
				width: (id_time_left.visible || id_time_right.visible) ? (parent.width * 0.75) : parent.width;
			}

			Text
			{
				id: id_time_right;				
				bottomPadding: padding_size_2;
				color: medium_grey;
				font.bold: true;
				font.pixelSize: font_size * 0.90;
				height: font_size + padding_size;
				horizontalAlignment: Text.AlignRight;
				rightPadding: padding_size;
				text: m_time.state;
				topPadding: padding_size_3;
				verticalAlignment: Text.AlignVCenter;
				visible: false;
				width: parent.width * 0.25;
			}
		}

		// Message
		Text
		{
			id: id_message;
			anchors.top: id_row.bottom;
			color: black;
			font.pixelSize: font_size;
			horizontalAlignment: m_self.enabled ? Text.AlignRight : Text.AlignLeft;
			rightPadding: padding_size;
			text: m_message.state;
			textFormat: Text.PlainText;
			verticalAlignment: Text.AlignVCenter;
			width: parent.width;
			wrapMode: Text.Wrap;
		}
	}

	MouseArea
	{
		id: id_mousearea;
		anchors.fill: parent;	

		// Handlers
		onPressAndHold:
		{
			id_view.currentIndex = index;

			// Menu			
			id_loader.source = "qrc:/qml/menu/MessageMenu.qml";
			id_loader.item.init(item);
			
			var pos = mapToItem(id_view, item.x, item.y);
			id_loader.item.x = (id_view.x + (id_view.width / 2)) - (id_loader.item.width / 2);
			id_loader.item.y = (pos.y + (item.height / 2)) - (id_loader.item.height / 2);
			id_loader.item.open();

			Vibrator.vibrate();

			mouse.accepted = true;
		}

		onPressed:
		{
			// HACK: We need chat to release its textfield input focus
			if (!id_view.activeFocus)
				id_chat.inactivate();
		}

		onReleased:
		{
			// Toggle time
			if (m_self.enabled)
				id_time_left.visible = !id_time_left.visible;
			else
				id_time_right.visible = !id_time_right.visible;
		}
	}

	// Row Background
	Rectangle
	{
		id: id_background;
		anchors
		{
			left: m_self.enabled ? id_item.left : id_color_left.right;
			right: m_self.enabled ? id_color_right.left : id_item.right;
			top: id_text.top;
		}
		height: id_text.height + padding_size_2;
		z: -1;
		color: light_grey;
		opacity: id_mousearea.pressed ? 0.25 : 0;
	}
}
