
import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

Item
{
	id: id_item;
	height: header_size;
	width: parent.width;

	//
	// HACK: Optimization for rendering
	//
	Item
	{
		id: m_muted;
		enabled: role_muted;
		visible: false;
	}

	Item
	{
		id: m_name;
		state: role_name;
		visible: false;
	}

	Rectangle
	{
		id: m_color;
		color: role_color;
		visible: false;
	}

	Rectangle
	{
		id: m_color_inverse;
		color: role_color_inverse;
		visible: false;
	}

	//  Functions
	function get_row()
	{
		return index;
	}

	// Functions
	function muted()
	{
		return m_muted.enabled;
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
		height: padding_size;
		width: parent.width;
	}

	// Color
	Rectangle
	{
		id: id_color;
		anchors.verticalCenter: parent.verticalCenter;
		color: "transparent";
		height: header_size;
		width: icon_size_2;

		Item
		{
			anchors
			{
				fill: parent;
				margins: ball_padding_size;
			}

			Rectangle
			{
				anchors.centerIn: parent;
				border
				{
					color: !m_muted.enabled ? (!id_mousearea.pressed ? m_color.color : m_color_inverse.color) : m_color.color;
					width: ball_highlight_size;
				}
				color: m_color.color;
				height: ball_size;
				radius: ball_size  * 0.5;
				width: ball_size;
			}

			Text
			{
				anchors.centerIn: parent;
				color: (role_contrast <= 0.5) ? white : grey;
				font.bold: true;
				font.pixelSize: small_icon_size;
				height: header_size;
				horizontalAlignment: Text.AlignHCenter;
				text: role_first;
				textFormat: Text.PlainText;
				width: ball_size;
				verticalAlignment: Text.AlignVCenter;
			}

			Text
			{
				anchors.centerIn: parent;
				color: m_color_inverse.color;
				font.family: "FontAwesome";
				font.pixelSize: header_size;
				height: header_size;
				horizontalAlignment: Text.AlignHCenter;
				text: fa_ban;
				verticalAlignment: Text.AlignVCenter;
				visible: m_muted.enabled;
				z: 1;
			}
		}
	}

	Item
	{
		id: id_text;

		anchors
		{
			left: id_color.right;
			right: id_item.right;
			rightMargin: padding_size;
			top: id_spacing.bottom;
		}
		height: parent.height - (id_spacing.height * 2);
		width: parent.width - id_color.width;

		// Name
		Text
		{
			id: id_name;
			color: grey;
			elide: Text.ElideRight;
			font.pixelSize: font_size;			
			height: parent.height;	
			rightPadding: padding_size;
			text: m_name.state;
			textFormat: Text.PlainText;
			verticalAlignment: Text.AlignVCenter;
			width: parent.width - padding_size;
		}
	}

	MouseArea
	{
		id: id_mousearea;
		anchors.fill: parent;

		// Handlers
		onClicked:
		{
			Group.message(index);
			id_swipe_view.push("qrc:/qml/Message.qml");
			mouse.accepted = true;
		}

		onPressAndHold:
		{
			id_view.currentIndex = index;

			// Menu
			id_loader.source = "qrc:/qml/menu/GroupMenu.qml";
			id_loader.item.init(item);

			var pos = mapToItem(id_view, item.x, item.y);
			id_loader.item.x = (id_view.x + (id_view.width / 2)) - (id_loader.item.width / 2);
			id_loader.item.y = (pos.y + (item.height / 2)) - (id_loader.item.height / 2);
			id_loader.item.open();

			Vibrator.vibrate();

			mouse.accepted = true;
		}
	}

	//Row Background
	Rectangle
	{
		id: id_background;
		anchors
		{			
			left: parent.left
			right: parent.right;
			top: parent.top;
		}
		height: header_size;
		z: -1;
		color: light_grey;
		opacity: id_mousearea.pressed? 0.25 : 0;
	}
}
