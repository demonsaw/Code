
import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

Item
{
	id: id_item;
	width: parent.width;
    height: row_size;

	//
	// HACK: Optimization for rendering
	//	
	Item
	{
		id: m_selected;
		enabled: role_selected
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

	// Functions
	function get_row()
	{
		return index;
	}

	// Client Colored Circle
	Rectangle
	{
		id: id_color;
		anchors.verticalCenter: parent.verticalCenter;
		color: "transparent";
        height: row_size;
		width: icon_size_2;

		// Circle
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
					color: !id_mousearea.pressed ? m_color.color : role_color_inverse;
					width: ball_highlight_size;
				}
				color:  m_color.color;
				height: ball_size;
				radius: ball_size  * 0.5;
				width: ball_size;
			}

			// First Character
			Text
			{
				anchors.centerIn: parent;
				color: (role_contrast <= 0.5) ? white : grey;
				font.bold: true;
				font.pixelSize: small_icon_size;
                height: row_size;
				horizontalAlignment: Text.AlignHCenter;
				text: role_first;
				textFormat: Text.PlainText;
				width: ball_size;
				verticalAlignment: Text.AlignVCenter;
			}		
		}
	}

	// Client Text
	Text
	{
		id: client_text;
		anchors
		{
			left: id_color.right;
			verticalCenter: id_color.verticalCenter;
		}
		font.family: "FontAwesome";
		font.pixelSize: title_size;
		color: grey;
        height: row_size;
		horizontalAlignment: Text.AlignLeft;
		topPadding: margin_height;
		bottomPadding: margin_height;
		rightPadding:margin_width;
		text: m_name.state;
		textFormat: Text.PlainText;
		verticalAlignment: Text.AlignVCenter;		
	}

	// Delete
	Rectangle
	{
		id: id_delete;
		anchors
		{
			right: parent.right;
			verticalCenter: parent.verticalCenter;
		}
		color: "transparent";
		height: parent.height;
		width: icon_size_2;
		visible: !m_selected.enabled  && (Client.size != 1);

		Text
		{
			id: id_delete_icon
			anchors.horizontalCenter: parent.horizontalCenter;
			color: id_delete_mousearea.pressed ? selected : grey;
			font.family: "FontAwesome";
			font.pixelSize: icon_size;
			height: parent.height;
			text: fa_delete;
			verticalAlignment: Text.AlignVCenter;
		}
		MouseArea
		{
			id: id_delete_mousearea;
			anchors.fill: parent;

			onClicked:
			{
				client_list.deleteClient(index, m_name.state);
			}
		}
	}

	// Row Background
	Rectangle
	{
		id: client_background;
		width: parent.width;
		height: parent.height;
		z: -1;
		color: !m_selected.enabled  ? light_grey : light_blue;
		opacity: !m_selected.enabled  ? (id_mousearea.pressed ? 0.25 : 0) : 0.25;
	}

	// Background MouseArea
	MouseArea
	{
		id: id_mousearea;
		anchors
		{
			bottom: parent.bottom;
			left: parent.left;
			right: id_delete.left;
			top: parent.top;
		}

		onClicked:
		{
			if (App.index() != index)
			{
				App.set_entity(index);
			
				id_chat.activate();
				id_clients.activate();
				App.save();
			}

			mouse.accepted = true;
		}
	}
}
