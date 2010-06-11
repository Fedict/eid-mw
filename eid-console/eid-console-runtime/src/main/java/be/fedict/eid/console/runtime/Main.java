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

package be.fedict.eid.console.runtime;

import java.awt.AWTException;
import java.awt.Desktop;
import java.awt.Image;
import java.awt.MenuItem;
import java.awt.PopupMenu;
import java.awt.SystemTray;
import java.awt.Toolkit;
import java.awt.TrayIcon;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;
import java.io.IOException;
import java.net.ServerSocket;
import java.net.URI;
import java.net.URISyntaxException;
import java.net.URL;

import javax.swing.JOptionPane;

import org.apache.commons.io.FileUtils;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.mortbay.jetty.Server;
import org.mortbay.jetty.bio.SocketConnector;
import org.mortbay.jetty.webapp.WebAppContext;

public class Main implements Runnable {

	private final static Log LOG = LogFactory.getLog(Main.class);

	private final MenuItem consoleMenuItem;

	private String consoleUrl;

	public Main() {
		SystemTray systemTray = SystemTray.getSystemTray();
		URL trayImageUrl = Main.class.getResource("tray.png");
		Image image = Toolkit.getDefaultToolkit().getImage(trayImageUrl);
		PopupMenu popup = new PopupMenu();
		this.consoleMenuItem = new MenuItem("eID Console");
		this.consoleMenuItem.setEnabled(false);
		this.consoleMenuItem.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent event) {
				if (false == Desktop.isDesktopSupported()) {
					LOG.error("Desktop not supported");
					return;
				}
				Desktop desktop = Desktop.getDesktop();
				try {
					desktop.browse(new URI(Main.this.consoleUrl));
				} catch (IOException e) {
					LOG.error("I/O error: " + e.getMessage(), e);
				} catch (URISyntaxException e) {
					LOG.error("URI error: " + e.getMessage(), e);
				}
			}
		});
		popup.add(this.consoleMenuItem);
		MenuItem aboutMenuItem = new MenuItem("About");
		popup.add(aboutMenuItem);
		aboutMenuItem.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent event) {
				JOptionPane.showMessageDialog(null, "eID Console\n"
						+ "Copyright 2010 FedICT", "About eID Console",
						JOptionPane.INFORMATION_MESSAGE);
			}
		});
		MenuItem exitMenuItem = new MenuItem("Exit");
		exitMenuItem.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent event) {
				System.exit(0);
			}
		});
		popup.add(exitMenuItem);
		TrayIcon trayIcon = new TrayIcon(image, "eID Console", popup);
		try {
			systemTray.add(trayIcon);
		} catch (AWTException e) {
			return;
		}
		trayIcon.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent event) {
				// TODO: show eID Console
			}
		});
		Thread runtimeThread = new Thread(this);
		runtimeThread.start();
	}

	public static void main(String[] args) {
		new Main();
	}

	public void run() {
		Server server = new Server();

		WebAppContext webAppContext = new WebAppContext();
		webAppContext.setContextPath("/eid-console");

		URL webappURL = Main.class.getResource("eid-console-webapp.war");
		if (null == webappURL) {
			LOG.fatal("could not find the web application WAR file");
			System.exit(1);
			return;
		}

		File tmpWarFile;
		try {
			tmpWarFile = File.createTempFile("eid-console-webapp-", ".war");
		} catch (IOException e) {
			LOG.fatal("could not create tmp WAR file");
			System.exit(1);
			return;
		}
		try {
			FileUtils.copyURLToFile(webappURL, tmpWarFile);
		} catch (IOException e) {
			LOG.fatal("could not write tmp WAR file");
			System.exit(1);
			return;
		}

		LOG.debug("tmp WAR file: " + tmpWarFile.getAbsolutePath());
		webAppContext.setWar(tmpWarFile.getAbsolutePath());
		server.setHandler(webAppContext);
		server.setStopAtShutdown(true);

		SocketConnector socketConnector = new SocketConnector();
		int port;
		try {
			port = getFreePort();
		} catch (Exception e) {
			LOG.fatal("could not get a free port");
			System.exit(1);
			return;
		}
		socketConnector.setPort(port);
		server.addConnector(socketConnector);

		try {
			LOG.debug("starting the Jetty servlet container");
			server.start();
		} catch (Exception e) {
			LOG.error("error starting the jetty servlet container: "
					+ e.getMessage(), e);
		}

		this.consoleUrl = "http://localhost:" + port + "/eid-console/";
		LOG.debug("URL: " + this.consoleUrl);

		this.consoleMenuItem.setEnabled(true);
	}

	private int getFreePort() throws Exception {
		ServerSocket serverSocket = new ServerSocket(0);
		int port = serverSocket.getLocalPort();
		serverSocket.close();
		return port;
	}
}
