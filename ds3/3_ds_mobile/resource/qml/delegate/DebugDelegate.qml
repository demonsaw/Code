
import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

Item
{
	// Variables
	readonly property bool m_first: (index == 0);

	id: id_item;
	height: id_spacing.height + id_text.height;
	width: parent.width;

	//
	// HACK: Optimization for rendering
	//
	Item
	{
		id: m_message;
		state: role_message;
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
		id: id_color;
		
		anchors
		{
			left: parent.left;			
			top: id_spacing.bottom;
		}
		color: red;
		height: id_name.height + id_message.height;
		width: color_size;
	}
	
	// Item
	Item
	{
		id: id_text;

		anchors
		{
			bottomMargin: padding_size;
			left: id_color.right;
			leftMargin: padding_size_2;
			right: id_item.right;
			rightMargin: padding_size;
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
				id: id_name;
				bottomPadding: padding_size_2;
				color: red;
				font.bold: true;
				font.pixelSize: font_size;
				height: font_size + padding_size_3;
				horizontalAlignment: Text.AlignLeft;
				rightPadding: padding_size;
				text: "Error";
				textFormat: Text.PlainText;
				topPadding: padding_size_3;
				verticalAlignment: Text.AlignVCenter;
				width: !id_time.visible ? parent.width : (parent.width * 0.75);
			}

			Text
			{
				id: id_time;				
				bottomPadding: padding_size_2;
				color: medium_grey;
				font.bold: true;
				font.pixelSize: font_size * 0.90;
				height: font_size + padding_size_3;
				horizontalAlignment: Text.AlignRight;
				rightPadding: padding_size;
				text: role_time;
				textFormat: Text.PlainText;
				topPadding: padding_size_3;
				verticalAlignment: Text.AlignVCenter;
				visible: true;
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
			rightPadding: padding_size;
			text: m_message.state;
			textFormat: Text.PlainText;
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
			id_loader.source = "qrc:/qml/menu/DebugMenu.qml";
			id_loader.item.init(item);
			
			var pos = mapToItem(id_view, item.x, item.y);
			id_loader.item.x = (id_view.x + (id_view.width / 2)) - (id_loader.item.width / 2);
			id_loader.item.y = (pos.y + (item.height / 2)) - (id_loader.item.height / 2);
			id_loader.item.open();

			Vibrator.vibrate();
			
			mouse.accepted = true;
		}

		onReleased:
		{
			// Toggle time
			id_time.visible = !id_time.visible;
		}
	}

	//Row Background
	Rectangle
	{
		id: id_background;
		anchors
		{
			left: id_color.right;
			right: id_item.right;
			top: id_text.top;
		}
		height: id_text.height + padding_size_2;
		z: -1;
		color: light_grey;
		opacity: id_mousearea.pressed ? 0.25 : 0;
	}
}
