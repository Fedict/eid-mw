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

package be.fedict.eidviewer.gui.helper;

import java.awt.Graphics2D;
import java.awt.Image;
import java.awt.Toolkit;
import java.awt.image.BufferedImage;
import javax.swing.ImageIcon;

/**
 *
 * @author frank
 */
public class ImageUtilities
{
    public static BufferedImage makeBufferedImage(Image image)
    {
        int width = image.getWidth(null);
        int height = image.getHeight(null);
        BufferedImage bufferedImage = new BufferedImage(width, height, BufferedImage.TYPE_BYTE_GRAY);
        Graphics2D g2d = bufferedImage.createGraphics();
        g2d.drawImage(image, 0, 0, width, height, null);
        return bufferedImage;
    }

    public static BufferedImage getBufferedImage(Class clazz, String iconsPath, String name)
    {
        return ImageUtilities.makeBufferedImage(getImage(clazz, iconsPath, name));
    }

    public static Image getImage(Class clazz, String iconsPath, String name)
    {
        return (new ImageIcon(Toolkit.getDefaultToolkit().getImage(clazz.getResource(iconsPath + name))).getImage());
    }
}
