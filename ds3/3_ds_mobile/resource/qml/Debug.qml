
import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

import "delegate"
import "header"
import "menu"

Page
{
	// Signal
	signal activate();
	onActivate:
	{		
		id_view.positionViewAtBeginning();
		id_view.currentIndex = 0;
	}

	// Loader
	Loader
	{
		id: id_loader;
		focus:true;

		// Variables
		property bool valid: item !== null
	}

	// Header
	header: DebugHeader
	{
		anchors.left: parent.left;
		anchors.top: parent.top;
		height: header_size;
		width: parent.width;
	}

	// Body
	Rectangle
	{
		anchors.fill: parent;
		color: white; 

		// Background
		Text
		{
			id: id_background;
			anchors
			{
				horizontalCenter: id_size.horizontalCenter;				
				verticalCenter: parent.verticalCenter;
			}
			color: watermark_color;
			font.bold: true;
			font.family: "FontAwesome";
			font.pixelSize: background_size;
			horizontalAlignment: Text.AlignHCenter;
			opacity: watermark_opacity;
			rightPadding: font_size / 2;
			text: fa_debug;
			verticalAlignment: Text.AlignVCenter;
		}

		// Size
		Text
		{
			id: id_size;
			anchors
			{
				fill: parent;
				topMargin: id_background.height * 0.90;
			}
			color: watermark_color;
			font
			{
				bold: true;
				pixelSize: title_size;
			}
			height: parent.height;
			horizontalAlignment: Text.AlignHCenter;
			opacity: size_opacity;
			text: Number.fromLocaleString(Debug.size);
			verticalAlignment: Text.AlignVCenter;
			width: parent.width;
		}

		// View
		ListView
		{
			id: id_view;
			
			anchors.fill: parent;
			boundsBehavior: Flickable.DragOverBounds;
			delegate: Loader
			{
				anchors 
				{
					left: parent.left;
					right: parent.right;
				}

				source: "qrc:/qml/delegate/DebugDelegate.qml";
			}
			height: parent.height;
			maximumFlickVelocity: max_vertical_velocity;
			model: error_model;
			orientation: Qt.Vertical;
			width: parent.width;

			// Header
			DebugListHeader
			{
				id: id_header
				y: -id_view.contentY - height
			}

			// Footer
			footer: Rectangle
			{
				color: "transparent";
				height: padding_size_2 + 1;
				width: parent.width;
			}

			// Handlers
			onCountChanged:
			{
				if (atYEnd)
				{
					var index = count - 1;
					positionViewAtEnd();
					currentIndex = index;
				}
			}

			onDragEnded:
			{
				if (id_header.modified)
					Debug.clear();
			}
		}
	}
}
