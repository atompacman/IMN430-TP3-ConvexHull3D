def getOrderedHorizonEdges(point_node, D):
	# D est la DCEL

	# Trouver les edges autour du groupe de faces en conflit avec point_node
	# Critere: Inclure un half-edge si son twin est sur une face non en conflit (invisible)

	# Optimisation: trouver un premier half-edge, et continuer le long de l'horizon

	horizon = []

	# Trouver un premier half-edge faisant partie de l'horizon
	trouve = False
	for face in point_node.conflits:
		# Boucler sur les half-edges de la face
		premier_half_edge = face.an_edge
		half_edge = premier_half_edge
		continuer = True 
		while continuer:
			# Si twin fait partie d'une face invisible, on a trouve
			adjac_face = half_edge.twin.face
			if adjac_face not in point_node.conflits:
				horizon.append(half_edge)
				trouve = True
				break
			# Suite de la boucle
			half_edge = half_edge.next
			continuer = half_edge != premier_half_edge
		if trouve:
			break

	# Trouver les half-edge suivants sur l'horizon
	# A partir d'un half-edge, trouver une face adjacente (non-visitee) ayant un half-edge sur l'horizon
	face = horizon[0].face
	faces_visitees = []
	while True:
		# Boucler sur les half-edges de la face
		premier_half_edge = face.an_edge
		half_edge = premier_half_edge
		continuer = True
		trouve = False
		while continuer:
			# Si twin fait partie d'une face invisible, il fait partie de l'horizon
			adjac_face = half_edge.twin.face
			if adjac_face not in faces_visitees and adjac_face not in point_node.conflits:
				horizon.append(half_edge)
				faces_visitees.append(adjac_face)
				trouve = True
				break
			# Suite de la boucle
			half_edge = half_edge.next
			continuer = half_edge != premier_half_edge
		if trouve:
			# On recommence avec la face du nouveau half-edge d'horizon
			face = half_edge.face
		else:
			# On a termine, car on a visite toutes les faces sur l'horizon
			break

	return horizon