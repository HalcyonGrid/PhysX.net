﻿using System;
using System.Collections.Generic;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace PhysX.Test
{
	[TestClass]
	public class CapsuleControllerTest : Test
	{
		[TestMethod]
		public void CreateAndDisposeCapsuleController()
		{
			using (var core = CreatePhysicsAndScene())
			{
				var controllerManager = core.Scene.CreateControllerManager();

				var material = core.Physics.CreateMaterial(0.1f, 0.1f, 0.1f);

				var desc = new BoxControllerDesc()
				{
					Extents = new Math.Vector3(5, 5, 5),
					Material = material
				};

				Assert.IsTrue(desc.IsValid());

				BoxController boxController;
				using (boxController = controllerManager.CreateController<BoxController>(desc))
				{
					Assert.IsNotNull(boxController);
					Assert.IsFalse(boxController.Disposed);
				}

				Assert.IsTrue(boxController.Disposed);
			}
		}
	}
}