﻿using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace InHomeSwitching
{
    /// <summary>
    /// Describes the movement of an image rectangle within a desktop frame.
    /// </summary>
    /// <remarks>
    /// Move regions are always non-stretched regions so the source is always the same size as the destination.
    /// </remarks>
    public struct MovedRegion
    {
        /// <summary>
        /// Gets the location from where the operating system copied the image region.
        /// </summary>
        public Point Source { get; internal set; }

        /// <summary>
        /// Gets the target region to where the operating system moved the image region.
        /// </summary>
        public Rectangle Destination { get; internal set; }
    }
}
