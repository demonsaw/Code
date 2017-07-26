
import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

Menu 
{
	// Variables
	property Item m_item: null;

	height: (menu_height * 2) + (line_size * 1);
	width: menu_width;

	// Functions
	function init(item)
	{
		m_item = item;
		id_mute.text = m_item.muted() ? "Unmute" : "Mute";
	}

	background: Rectangle
	{		
		border.color: light_grey;
		border.width: line_size;
		color: white;
		width: parent.width;
	}

	MenuItem
	{
		font.pixelSize: font_size;
		height: menu_height;
		padding: font_size;
		text: "Message";
		width: parent.width;

		onTriggered:
		{
			Envelope.message(m_item.get_row());
			id_swipe_view.push("qrc:/qml/Message.qml");
		}
	}

	Rectangle
	{
		color: grey;
		height: line_size;
		opacity: 0.06;
		width: parent.width;
	}

	MenuItem 
	{
		id: id_mute;
		font.pixelSize: font_size;
		height: menu_height;
		padding: font_size;		
		width: parent.width;

		onTriggered: Envelope.mute(m_item.get_row(), !m_item.muted());
	}
}
