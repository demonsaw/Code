
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
		text: "Copy";
		width: parent.width;

		onTriggered: Clipboard.text = m_item.get_message();
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
		font.pixelSize: font_size;
		height: menu_height;
		padding: font_size;
		text: "Clear";
		width: parent.width;

		onTriggered: Debug.clear();
	}
}
