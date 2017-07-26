
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

	signal inactivate();
	onInactivate:
	{
		id_view.forceActiveFocus();
		Qt.inputMethod.hide();
	}

	// Loader
	Loader
	{
		id: id_loader;
        active: id_swipe_view.currentIndex == page_loaded; // Close loader(dialog) when user swipes away.
		focus:true;

		// Variables
		property bool valid: item !== null
	}

	// Header
	header: MessageHeader
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
				horizontalCenter: parent.horizontalCenter;
				verticalCenter: parent.verticalCenter;
			}
			color: watermark_color;
			font.bold: true;
			font.family: "FontAwesome";
			font.pixelSize: background_size;			
			horizontalAlignment: Text.AlignHCenter;
			opacity: watermark_opacity;
			rightPadding: font_size / 2;
			text: fa_message;
			verticalAlignment: Text.AlignVCenter;
		}

		// Size
		Text
		{
			anchors
			{				
				bottomMargin: id_background.height / 4;
				fill: parent;
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
			text: Number.fromLocaleString(Message.size);
			verticalAlignment: Text.AlignVCenter;
			width: parent.width;
		}

		// View
		ListView
		{
			id: id_view;

			anchors.fill: parent;
			bottomMargin: padding_size;
			boundsBehavior: Flickable.DragOverBounds;
			cacheBuffer: id_root.height;
			delegate: Loader
			{
				anchors 
				{
					left: parent.left;
					right: parent.right;
				}

				source: "qrc:/qml/delegate/MessageDelegate.qml";
			}
			focus: true;
			height: parent.height;			
			maximumFlickVelocity: max_vertical_velocity;
			model: Message.get_model();
			orientation: Qt.Vertical;
			verticalLayoutDirection : ListView.BottomToTop;
			width: parent.width;

			// Header
			/*ChatListHeader
			{
				id: id_header;
				y: -id_view.contentY;
			}*/

			// Footer
			header: Rectangle
			{
				color: "transparent";
				height: padding_size;
				width: parent.width;
			}

			MouseArea
			{
				anchors.fill: parent;
				propagateComposedEvents: true;

				// Handlers				
				onPressed:
				{
					forceActiveFocus();
					Qt.inputMethod.hide();
					mouse.accepted = true;
				}
				onDoubleClicked: mouse.accepted = true;
				onPressAndHold: mouse.accepted = false;
				z: -1;
			}

			// Handlers
			/*onDragEnded:
			{
				if (id_header.modified)
				{
					Message.clear();
					id_footer.clear();
				}					
			}*/
		}
	}

	// Footer
	footer: Rectangle
	{
		id: id_footer;
		color: white;
		height: footer_size;
		width: parent.width;

		function clear()
		{
			id_input.text = "";
        }

        Rectangle
        {
            id: id_client_color;
            anchors
            {
                top: parent.top;
                bottom: parent.bottom;
                left: parent.left;
            }
            height: padding_size_2 - 1;
            color: Client.color;
            width: color_size;

        }

		TextField 
		{
			id: id_input

            anchors
            {
                bottom: parent.bottom;
                left: parent.left;
                leftMargin: margin_width;
                rightMargin: margin_width / 2;
                right: id_icon.left;
                top: parent.top;
            }
			color: black;
			font.pixelSize: font_size;
			height: footer_size;
			horizontalAlignment: Text.AlignLeft;			
			maximumLength: 256;
			leftPadding: 0;
			topPadding: padding_size;
			bottomPadding: padding_size;
            placeholderText: Message.name;
			selectionColor: light_blue;
			selectedTextColor: white;			
			verticalAlignment: Text.AlignVCenter;
			width: parent.width - id_icon.width;
            wrapMode: TextInput.Wrap;

            function clear()
            {
                id_input.text = "";
            }

            function paste()
            {
                var beginning = id_input.text.substring(0, id_input.cursorPosition);
                var end = id_input.text.substring(id_input.cursorPosition, id_input.length);
                id_input.text = beginning + Clipboard.text + end;
            }

			background: Rectangle
			{
				color: white;
			}

            /*MouseArea
            {
                id: id_mousearea;
                anchors.fill: parent;

                function updateCursorPosition()
                {
                    // HACK: Work-around for Android predictive text bullshit
                    Qt.inputMethod.commit();

                    id_input.cursorPosition = id_input.positionAt(mouseX, mouseY);
                }

                // Handlers
                onClicked:
                {
                    if (!id_input.activeFocus)
                        id_input.forceActiveFocus();

                    Qt.inputMethod.show();

                    updateCursorPosition();
                }
                onPressAndHold:
                {
                    updateCursorPosition();

                    // Menu
                    id_loader.source = "qrc:/qml/menu/ClipboardMenu.qml";
                    id_loader.item.init(id_input);

                    var pos = mapToItem(id_view, id_input.x, id_input.y);
                    id_loader.item.x = (id_view.x + (id_view.width / 2)) - (id_loader.item.width / 2);
                    id_loader.item.y = (pos.y + (id_input.height / 2)) - (id_loader.item.height / 2);
                    id_loader.item.focus = false;
                    id_loader.item.open();

                    Vibrator.vibrate();
                }
            }*/

			// Handlers
			Keys.onPressed:
			{
				if ((event.key == Qt.Key_Enter) || (event.key == Qt.Key_Return)) 
				{					
					if (Message.enabled())
					{
						// HACK: Work-around for Android predictive text bullshit
						Qt.inputMethod.commit();

						if (id_input.text.length > 0)
						{
							var result = Message.add(id_input.text);
							if (result)
								id_input.text = "";					
						}
					}

					event.accepted = true;
				}
			}
        }
        Text
        {
            id: id_icon;
            anchors
            {
                right: parent.right;
                rightMargin: margin_width / 2;
            }
            color: (id_input.preeditText !== "" || id_input.length > 0) ? blue : medium_grey; // HACK: So we can tell if there's uncommitted text
            font.bold: true;
            font.family: "FontAwesome";
            font.pixelSize: cursor_size;
            height: footer_size;
            horizontalAlignment: Text.AlignHCenter;
            text: fa_prompt;
            verticalAlignment: Text.AlignVCenter;
            width: icon_size;

            MouseArea
            {
                anchors
                {
                    top: parent.top;
                    bottom: parent.bottom;
                    right: parent.right;
                }
                width: parent.width + margin_width / 2;

                onClicked:
                {
                    if (Message.enabled())
                    {
                        // HACK: Work-around for Android predictive text bullshit
                        Qt.inputMethod.commit();

                        if (id_input.text.length > 0)
                        {
                            var result = Message.add(id_input.text);
                            if (result)
                                id_input.text = "";
                        }
                    }
                }
            }
        }
	}
}
