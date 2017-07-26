import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1

import "../delegate/"
import "../menu"

Item
{
	id: client_list;

	// Variables
	property alias id_confirm_delete: id_confirm_delete;
	property int m_row;

	// Functions
	function deleteClient(row, name)
	{
		m_row = row;
		id_confirm_delete.title = "Remove";
		id_confirm_delete.text = "Remove " + name + "?";
		id_confirm_delete.visible = true;
	}

	ListView 
	{
		id: id_view;		
		boundsBehavior: Flickable.StopAtBounds;
		delegate: Loader
		{
			anchors 
			{
				left: parent.left;
				right: parent.right;
			}

			source: "qrc:/qml/delegate/ClientDelegate.qml";
		}
		focus: true;
		model: client_model;
		orientation: Qt.Vertical;
		width: parent.width;
	}

	// Confirm Dialog
	MessageDialog 
	{
		id: id_confirm_delete;		
		standardButtons: StandardButton.No | StandardButton.Yes;

		onYes:
		{
			Client.remove(m_row);
			App.save();
            Toast.show("Client removed");
		}

		onNo:
		{
			visible = false;
		}
	}
}
