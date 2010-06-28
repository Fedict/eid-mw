/*
 * eID Middleware Project.
 * Copyright (C) 2010 FedICT.
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version
 * 3.0 as published by the Free Software Foundation.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, see 
 * http://www.gnu.org/licenses/.
 */

package be.fedict.eid.console.client;

import com.google.gwt.core.client.EntryPoint;
import com.google.gwt.core.client.GWT;
import com.google.gwt.event.dom.client.ClickEvent;
import com.google.gwt.event.dom.client.ClickHandler;
import com.google.gwt.event.logical.shared.SelectionEvent;
import com.google.gwt.event.logical.shared.SelectionHandler;
import com.google.gwt.http.client.Request;
import com.google.gwt.http.client.RequestBuilder;
import com.google.gwt.http.client.RequestCallback;
import com.google.gwt.http.client.RequestException;
import com.google.gwt.http.client.Response;
import com.google.gwt.json.client.JSONObject;
import com.google.gwt.json.client.JSONParser;
import com.google.gwt.json.client.JSONValue;
import com.google.gwt.maps.client.InfoWindowContent;
import com.google.gwt.maps.client.MapWidget;
import com.google.gwt.maps.client.Maps;
import com.google.gwt.maps.client.control.LargeMapControl;
import com.google.gwt.maps.client.geocode.Geocoder;
import com.google.gwt.maps.client.geocode.LatLngCallback;
import com.google.gwt.maps.client.geom.LatLng;
import com.google.gwt.maps.client.overlay.GeoXmlLoadCallback;
import com.google.gwt.maps.client.overlay.GeoXmlOverlay;
import com.google.gwt.maps.client.overlay.Marker;
import com.google.gwt.user.client.Command;
import com.google.gwt.user.client.Event;
import com.google.gwt.user.client.Window;
import com.google.gwt.user.client.ui.Button;
import com.google.gwt.user.client.ui.DeckPanel;
import com.google.gwt.user.client.ui.DecoratedTabPanel;
import com.google.gwt.user.client.ui.DialogBox;
import com.google.gwt.user.client.ui.DockPanel;
import com.google.gwt.user.client.ui.FlexTable;
import com.google.gwt.user.client.ui.Frame;
import com.google.gwt.user.client.ui.HTML;
import com.google.gwt.user.client.ui.Image;
import com.google.gwt.user.client.ui.Label;
import com.google.gwt.user.client.ui.MenuBar;
import com.google.gwt.user.client.ui.RootPanel;
import com.google.gwt.user.client.ui.VerticalPanel;

public class Application implements EntryPoint {

	private static final int DECK_FRAME = 0;
	private static final int DECK_IDENTIFICATION_RESULT = 1;
	private static final int DECK_PIN_VERIFY_RESULT = 2;
	private static final int DECK_DIAGNOSTIC = 3;

	private Frame contentFrame;
	private DeckPanel contentPanel;
	private FlexTable identityTable;
	private FlexTable addressTable;
	private FlexTable cardTable;
	private FlexTable diagnosticTable;
	private MapWidget map;
	private Frame diagnosticFrame;

	private static Application application;

	Command aboutCommand = new Command() {

		private DialogBox dialogBox;

		ClickHandler closeHandler = new ClickHandler() {

			public void onClick(ClickEvent event) {
				dialogBox.hide();
			}
		};

		public void execute() {
			this.dialogBox = new DialogBox(false);
			this.dialogBox.setAnimationEnabled(true);
			this.dialogBox.setText("About eID Console");
			this.dialogBox.center();

			HTML message = new HTML("eID Console<br/>Copyright 2010 FedICT");

			VerticalPanel panel = new VerticalPanel();
			panel.add(message);
			Button closebutton = new Button("Close", this.closeHandler);
			panel.add(closebutton);
			this.dialogBox.setWidget(panel);

			this.dialogBox.show();
		}
	};

	Command readCommand = new Command() {
		public void execute() {
			Application.this.showHtmlPage("read-eid.html");
		}
	};

	Command verifyPinCommand = new Command() {
		public void execute() {
			Application.this.showHtmlPage("verify-pin.html");
		}
	};

	Command changePinCommand = new Command() {
		public void execute() {
			Application.this.showHtmlPage("change-pin.html");
		}
	};

	Command validateCommand = new Command() {
		public void execute() {
			Application.this
					.showHtmlUrl("https://www.e-contract.be/eid-trust-service-portal/");
		}
	};

	Command applicationsCommand = new Command() {
		public void execute() {
			Application.this.showHtmlPage("applications.html");
		}
	};

	Command diagnosticCommand = new Command() {
		public void execute() {
			String diagnosticUrl = GWT.getModuleBaseURL() + "diagnostic.html";
			if (false == diagnosticUrl.equals(Application.this.diagnosticFrame
					.getUrl())) {
				Application.this.diagnosticFrame.setUrl(diagnosticUrl);
			}
			Application.this.contentPanel.showWidget(DECK_DIAGNOSTIC);
			Application.this.clearDiagnosticResults();
		}
	};

	private int diagnosticIdx;

	private void clearDiagnosticResults() {
		this.diagnosticTable.removeAllRows();
		this.diagnosticTable.setHTML(0, 0, "<b>Test ID</b>");
		this.diagnosticTable.setHTML(0, 1, "<b>Test Description</b>");
		this.diagnosticTable.setHTML(0, 2, "<b>Test Result</b>");
		this.diagnosticTable.setHTML(0, 3, "<b>Test Result Description</b>");
		this.diagnosticIdx = 1;
	}

	private void addDiagnosticResult(String testId, String testDescription,
			boolean testResult, String testResultDescription) {
		this.diagnosticTable.setText(this.diagnosticIdx, 0, testId);
		this.diagnosticTable.setText(this.diagnosticIdx, 1, testDescription);
		this.diagnosticTable.setHTML(this.diagnosticIdx, 2,
				testResult ? "<font color=\"green\">success</font>"
						: "<font color=\"red\">failure</font>");
		this.diagnosticTable.setText(this.diagnosticIdx, 3,
				testResultDescription);
		this.diagnosticIdx++;
	}

	private void showHtmlPage(String htmlPage) {
		String htmlUrl = GWT.getModuleBaseURL() + htmlPage;
		showHtmlUrl(htmlUrl);
	}

	private void showHtmlUrl(String htmlUrl) {
		this.contentFrame.setUrl(htmlUrl);
		// DECK_FRAME shown via onBrowserEvent
	}

	public void onModuleLoad() {
		exportStaticMethod();

		/*
		 * Menu
		 */
		MenuBar menuBar = new MenuBar();
		menuBar.addStyleName("MenuBar");

		MenuBar fileMenuBar = new MenuBar(true);
		fileMenuBar.addItem("Read eID", this.readCommand);
		fileMenuBar.addItem("Validate Certificates", this.validateCommand);
		fileMenuBar.addItem("eID Applications", this.applicationsCommand);
		fileMenuBar.addItem("eID Diagnostic Tests", this.diagnosticCommand);
		menuBar.addItem("File", fileMenuBar);

		MenuBar pinMenuBar = new MenuBar(true);
		menuBar.addItem("PIN", pinMenuBar);
		pinMenuBar.addItem("Verify PIN", this.verifyPinCommand);
		pinMenuBar.addItem("Change PIN", this.changePinCommand);

		MenuBar helpMenuBar = new MenuBar(true);
		helpMenuBar.addItem("About", this.aboutCommand);
		menuBar.addItem("Help", helpMenuBar);

		RootPanel.get().add(menuBar);

		/*
		 * Content
		 */
		this.contentPanel = new DeckPanel();
		this.contentPanel.setWidth("100%");
		this.contentPanel.setHeight("100%"); // IE8 doesn't support inherent
		RootPanel.get().add(this.contentPanel);

		this.contentFrame = new Frame() {
			// http://code.google.com/p/google-web-toolkit/issues/detail?id=1720
			@Override
			public void onBrowserEvent(Event event) {
				/*
				 * We do this trick to be sure that an Applet on the previous
				 * loaded page won't be activated again by the JRE plugin.
				 */
				if (Event.ONLOAD == event.getTypeInt()) {
					Application.this.contentPanel.showWidget(DECK_FRAME);
				}
				super.onBrowserEvent(event);
			}
		};
		this.contentFrame.sinkEvents(Event.ONLOAD);
		this.contentPanel.add(this.contentFrame);

		DecoratedTabPanel identificationPanel = new DecoratedTabPanel();
		identificationPanel.setAnimationEnabled(true);
		this.contentPanel.add(identificationPanel);

		VerticalPanel identityPanel = new VerticalPanel();
		identificationPanel.add(identityPanel, "Identity");
		this.identityTable = new FlexTable();
		this.identityTable.setText(0, 0, "Name");
		this.identityTable.setText(1, 0, "First Name");
		this.identityTable.setText(2, 0, "Photo");
		this.identityTable.setText(3, 0, "Gender");
		this.identityTable.setText(4, 0, "Date of birth");
		this.identityTable.setText(5, 0, "National Number");
		this.identityTable.setText(6, 0, "Middle Name");
		this.identityTable.setText(7, 0, "Place of birth");
		identityPanel.add(this.identityTable);

		this.addressTable = new FlexTable();
		VerticalPanel addressPanel = new VerticalPanel();
		addressPanel.add(this.addressTable);
		identificationPanel.add(addressPanel, "Address");
		this.addressTable.setText(0, 0, "Street and number");
		this.addressTable.setText(1, 0, "Municipality");
		this.addressTable.setText(2, 0, "ZIP");

		this.cardTable = new FlexTable();
		VerticalPanel cardPanel = new VerticalPanel();
		cardPanel.add(this.cardTable);
		identificationPanel.add(cardPanel, "Card");
		this.cardTable.setText(0, 0, "Card Number");
		this.cardTable.setText(1, 0, "Chip Number");
		this.cardTable.setText(2, 0, "Delivery Municipality");
		this.cardTable.setText(3, 0, "Validity Begin Date");
		this.cardTable.setText(4, 0, "Validity End Date");

		identificationPanel.selectTab(0);

		final VerticalPanel dock = new VerticalPanel();
		identificationPanel.add(dock, "Location");
		dock.setHeight("500px");
		dock.setWidth("500px");
		Maps.loadMapsApi("", "2", false, new Runnable() {
			public void run() {
				Application.this.map = new MapWidget();
				Application.this.map.setSize("500px", "500px");
				Application.this.map.addControl(new LargeMapControl());
				dock.add(Application.this.map);
			}
		});
		identificationPanel
				.addSelectionHandler(new SelectionHandler<Integer>() {

					public void onSelection(SelectionEvent<Integer> tabIdx) {
						if (3 == tabIdx.getSelectedItem()) {
							Application.this.map.checkResizeAndCenter();
						}
					}
				});

		VerticalPanel pinVerifyResultPanel = new VerticalPanel();
		this.contentPanel.add(pinVerifyResultPanel);
		pinVerifyResultPanel.add(new Label("PIN verification was successful."));

		DockPanel diagnosticPanel = new DockPanel();
		this.contentPanel.add(diagnosticPanel);
		diagnosticPanel.add(new HTML("<h1>eID Diagnostic Tests</h1>"),
				DockPanel.NORTH);
		this.diagnosticFrame = new Frame();
		this.diagnosticFrame.setWidth("100%");
		this.diagnosticFrame.setHeight("450px");
		diagnosticPanel.add(this.diagnosticFrame, DockPanel.SOUTH);
		this.diagnosticTable = new FlexTable();
		diagnosticPanel.add(this.diagnosticTable, DockPanel.NORTH);

		Application.application = this;
	}

	private static class IdentityRequestCallback implements RequestCallback {

		public void onError(Request request, Throwable throwable) {
			GWT.log("identity request error", throwable);
		}

		public void onResponseReceived(Request request, Response response) {
			if (response.getStatusCode() != 200) {
				Window.alert("Sorry, there was an error...");
				return;
			}
			String responseText = response.getText();
			JSONValue jsonValue = JSONParser.parse(responseText);
			JSONObject jsonObject = jsonValue.isObject();
			JSONValue identityJSONValue = jsonObject.get("identity");
			JSONObject identityJSONObject = identityJSONValue.isObject();
			final String name = identityJSONObject.get("name").isString()
					.stringValue();
			final String firstName = identityJSONObject.get("firstName")
					.isString().stringValue();
			String gender = identityJSONObject.get("gender").isString()
					.stringValue();
			String dateOfBirth = identityJSONObject.get("dateOfBirth")
					.isString().stringValue();
			String nationalNumber = identityJSONObject.get("nationalNumber")
					.isString().stringValue();
			String middleName = identityJSONObject.get("middleName").isString()
					.stringValue();
			String placeOfBirth = identityJSONObject.get("placeOfBirth")
					.isString().stringValue();
			Application.application.identityTable.setText(0, 1, name);
			Application.application.identityTable.setText(1, 1, firstName);
			Image image = new Image(GWT.getModuleBaseURL() + "photo.jpg");
			Application.application.identityTable.setWidget(2, 1, image);
			Application.application.identityTable.setText(3, 1, gender);
			Application.application.identityTable.setText(4, 1, dateOfBirth);
			Application.application.identityTable.setText(5, 1, nationalNumber);
			Application.application.identityTable.setText(6, 1, middleName);
			Application.application.identityTable.setText(7, 1, placeOfBirth);

			JSONValue addressJSONValue = jsonObject.get("address");
			JSONObject addressJSONObject = addressJSONValue.isObject();
			String streetAndNumber = addressJSONObject.get("streetAndNumber")
					.isString().stringValue();
			String municipality = addressJSONObject.get("municipality")
					.isString().stringValue();
			String zip = addressJSONObject.get("zip").isString().stringValue();
			Application.application.addressTable.setText(0, 1, streetAndNumber);
			Application.application.addressTable.setText(1, 1, municipality);
			Application.application.addressTable.setText(2, 1, zip);

			JSONValue cardJSONValue = jsonObject.get("card");
			JSONObject cardJSONObject = cardJSONValue.isObject();
			String cardNumber = cardJSONObject.get("cardNumber").isString()
					.stringValue();
			String chipNumber = cardJSONObject.get("chipNumber").isString()
					.stringValue();
			String cardDeliveryMunicipality = cardJSONObject.get(
					"cardDeliveryMunicipality").isString().stringValue();
			String cardValidityDateBegin = cardJSONObject.get(
					"cardValidityDateBegin").isString().stringValue();
			String cardValidityDateEnd = cardJSONObject.get(
					"cardValidityDateEnd").isString().stringValue();
			Application.application.cardTable.setText(0, 1, cardNumber);
			Application.application.cardTable.setText(1, 1, chipNumber);
			Application.application.cardTable.setText(2, 1,
					cardDeliveryMunicipality);
			Application.application.cardTable.setText(3, 1,
					cardValidityDateBegin);
			Application.application.cardTable
					.setText(4, 1, cardValidityDateEnd);

			Geocoder geocoder = new Geocoder();
			final String address = streetAndNumber + ", " + zip + " "
					+ municipality;
			geocoder.getLatLng(address, new LatLngCallback() {

				public void onSuccess(LatLng point) {
					MapWidget map = Application.application.map;
					map.clearOverlays();
					Marker marker = new Marker(point);
					map.addOverlay(marker);
					map.setCenter(point, 16);
					map.getInfoWindow().open(
							marker,
							new InfoWindowContent(new HTML(firstName + " "
									+ name + "<br>" + address)));
					String eidFeed = "http://reap.rovin.be/eid.xml";
					GeoXmlOverlay.load(eidFeed, new GeoXmlLoadCallback() {
						@Override
						public void onSuccess(String url, GeoXmlOverlay overlay) {
							MapWidget map = Application.application.map;
							map.addOverlay(overlay);
						}

						@Override
						public void onFailure(String url, Throwable caught) {
							// woops
						}
					});
				}

				public void onFailure() {
					MapWidget map = Application.application.map;
					map.clearOverlays();
				}
			});
		}
	}

	public static void showIdentity() {
		RequestBuilder requestBuilder = new RequestBuilder(RequestBuilder.GET,
				GWT.getModuleBaseURL() + "identity.json");

		try {
			requestBuilder.sendRequest("", new IdentityRequestCallback());
		} catch (RequestException e) {
			GWT.log("Could not send identity request", e);
		}

		Application application = Application.application;
		application.contentPanel.showWidget(DECK_IDENTIFICATION_RESULT);
	}

	public static void showMain() {
		Application application = Application.application;
		application.showHtmlPage("main.html");
	}

	public static void showPinVerifyResult() {
		Application application = Application.application;
		application.contentPanel.showWidget(DECK_PIN_VERIFY_RESULT);
	}

	public static void diagnosticDone() {
		// TODO
	}

	public static void diagnosticCallback(String testId,
			String testDescription, boolean testResult,
			String testResultDescription) {
		Application application = Application.application;
		application.addDiagnosticResult(testId, testDescription, testResult,
				testResultDescription);
	}

	public static native void exportStaticMethod() /*-{
													$wnd.showIdentity = $entry(@be.fedict.eid.console.client.Application::showIdentity());
													$wnd.showMain = $entry(@be.fedict.eid.console.client.Application::showMain());
													$wnd.showPinVerifyResult = $entry(@be.fedict.eid.console.client.Application::showPinVerifyResult());
													$wnd.diagnosticDone = $entry(@be.fedict.eid.console.client.Application::diagnosticDone());
													$wnd.diagnosticCallback = $entry(@be.fedict.eid.console.client.Application::diagnosticCallback(Ljava/lang/String;Ljava/lang/String;ZLjava/lang/String;));
													}-*/;
}
