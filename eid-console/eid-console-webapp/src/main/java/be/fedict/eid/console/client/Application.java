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
import com.google.gwt.http.client.Request;
import com.google.gwt.http.client.RequestBuilder;
import com.google.gwt.http.client.RequestCallback;
import com.google.gwt.http.client.RequestException;
import com.google.gwt.http.client.Response;
import com.google.gwt.json.client.JSONObject;
import com.google.gwt.json.client.JSONParser;
import com.google.gwt.json.client.JSONValue;
import com.google.gwt.user.client.Command;
import com.google.gwt.user.client.Window;
import com.google.gwt.user.client.ui.Button;
import com.google.gwt.user.client.ui.DeckPanel;
import com.google.gwt.user.client.ui.DialogBox;
import com.google.gwt.user.client.ui.FlexTable;
import com.google.gwt.user.client.ui.Frame;
import com.google.gwt.user.client.ui.HTML;
import com.google.gwt.user.client.ui.MenuBar;
import com.google.gwt.user.client.ui.RootPanel;
import com.google.gwt.user.client.ui.TabPanel;
import com.google.gwt.user.client.ui.VerticalPanel;

public class Application implements EntryPoint {

	private static final int DECK_FRAME = 0;
	private static final int DECK_IDENTIFICATION_RESULT = 1;

	private Frame contentFrame;
	private DeckPanel contentPanel;
	private FlexTable identityTable;
	private FlexTable addressTable;

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
			Application.this.contentFrame.setUrl(GWT.getModuleBaseURL()
					+ "read-eid.html");
			Application.this.contentPanel.showWidget(DECK_FRAME);
		}
	};

	Command validateCommand = new Command() {
		public void execute() {
			Application.this.contentFrame
					.setUrl("https://www.e-contract.be/eid-trust-service-portal/");
			Application.this.contentPanel.showWidget(DECK_FRAME);
		}
	};

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
		menuBar.addItem("File", fileMenuBar);

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

		this.contentFrame = new Frame();
		this.contentPanel.add(this.contentFrame);

		TabPanel identificationPanel = new TabPanel();
		this.contentPanel.add(identificationPanel);
		VerticalPanel identityPanel = new VerticalPanel();
		identificationPanel.add(identityPanel, "Identity");
		this.identityTable = new FlexTable();
		this.identityTable.setText(0, 0, "Name");
		this.identityTable.setText(1, 0, "First Name");
		identityPanel.add(this.identityTable);
		this.addressTable = new FlexTable();
		VerticalPanel addressPanel = new VerticalPanel();
		addressPanel.add(this.addressTable);
		identificationPanel.add(addressPanel, "Address");
		this.addressTable.setText(0, 0, "Street and number");
		this.addressTable.setText(1, 0, "Municipality");
		identificationPanel.selectTab(0);

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
			String name = identityJSONObject.get("name").isString()
					.stringValue();
			String firstName = identityJSONObject.get("firstName").isString()
					.stringValue();
			Application.application.identityTable.setText(0, 1, name);
			Application.application.identityTable.setText(1, 1, firstName);

			JSONValue addressJSONValue = jsonObject.get("address");
			JSONObject addressJSONObject = addressJSONValue.isObject();
			String streetAndNumber = addressJSONObject.get("streetAndNumber")
					.isString().stringValue();
			String municipality = addressJSONObject.get("municipality")
					.isString().stringValue();
			Application.application.addressTable.setText(0, 1, streetAndNumber);
			Application.application.addressTable.setText(1, 1, municipality);
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

	public static native void exportStaticMethod() /*-{
													$wnd.showIdentity = $entry(@be.fedict.eid.console.client.Application::showIdentity());
													}-*/;
}
